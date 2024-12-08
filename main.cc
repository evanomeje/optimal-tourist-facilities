#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>

// Structure to represent a location with x, y coordinates and traffic intensity
struct Location {
    double x;
    double y;
    int traffic;  // Estimated number of people per hour
};

// Utility function to calculate Euclidean distance between two locations
double calculateDistance(const Location& a, const Location& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// BathroomOptimizer class to determine optimal bathroom placements
class BathroomOptimizer {
private:
    std::vector<Location> hotspots;       // High traffic areas
    std::vector<Location> bathrooms;      // Existing bathrooms
    double minDistance;                    // Minimum distance between bathrooms

    // Greedy algorithm for bathroom placement
    std::vector<Location> greedyPlacement(int numToilets, double maxDistance) {
        std::vector<Location> suggestedBathrooms;
        std::vector<bool> covered(hotspots.size(), false);

        for (int t = 0; t < numToilets; ++t) {
            double bestScore = -1.0;
            int bestIndex = -1;

            for (size_t i = 0; i < hotspots.size(); ++i) {
                if (covered[i]) continue;  // Skip already covered hotspots

                // Calculate score based on traffic and distance to existing bathrooms
                double score = hotspots[i].traffic;
                bool valid = true;

                for (const auto& bathroom : bathrooms) {
                    double dist = calculateDistance(hotspots[i], bathroom);
                    if (dist < minDistance) {
                        score -= 1000.0;  // Heavy penalty for proximity
                        valid = false;
                        break;
                    }
                }

                if (valid && score > bestScore) {
                    bestScore = score;
                    bestIndex = i;
                }
            }

            if (bestIndex != -1) {
                Location selected = hotspots[bestIndex];
                suggestedBathrooms.push_back(selected);
                bathrooms.push_back(selected);

                // Update coverage
                for (size_t i = 0; i < hotspots.size(); ++i) {
                    double dist = calculateDistance(selected, hotspots[i]);
                    if (dist <= maxDistance) {
                        covered[i] = true;
                    }
                }
            } else {
                break;  // No valid spots found
            }
        }

        return suggestedBathrooms;
    }

    // K-Means clustering algorithm for bathroom placement
    std::vector<Location> kMeansClustering(int numToilets, double maxDistance, int maxIterations = 100) {
        std::vector<Location> centers;
        std::vector<Location> suggestedBathrooms;
        std::vector<std::vector<Location>> clusters(numToilets);

        // Initialize centers randomly from hotspots
        srand(time(0));
        for (int i = 0; i < numToilets; ++i) {
            centers.push_back(hotspots[rand() % hotspots.size()]);
        }

        bool converged = false;
        int iterations = 0;

        while (!converged && iterations < maxIterations) {
            // Assign hotspots to the nearest center
            clusters.assign(numToilets, std::vector<Location>());
            for (const auto& hotspot : hotspots) {
                double minDist = std::numeric_limits<double>::max();
                int bestCluster = 0;
                for (int c = 0; c < numToilets; ++c) {
                    double dist = calculateDistance(hotspot, centers[c]);
                    if (dist < minDist) {
                        minDist = dist;
                        bestCluster = c;
                    }
                }
                clusters[bestCluster].push_back(hotspot);
            }

            // Recompute centers
            converged = true;
            for (int c = 0; c < numToilets; ++c) {
                if (clusters[c].empty()) continue;

                double sumX = 0.0, sumY = 0.0, sumTraffic = 0;
                for (const auto& loc : clusters[c]) {
                    sumX += loc.x;
                    sumY += loc.y;
                    sumTraffic += loc.traffic;
                }
                Location newCenter;
                newCenter.x = sumX / clusters[c].size();
                newCenter.y = sumY / clusters[c].size();
                newCenter.traffic = sumTraffic;  // Not used in distance calculation

                // Check for convergence
                if (calculateDistance(newCenter, centers[c]) > 1e-3) {
                    converged = false;
                }

                centers[c] = newCenter;
            }

            iterations++;
        }

        // Final centers are suggested bathroom locations
        for (const auto& center : centers) {
            // Ensure minimum distance from existing bathrooms
            bool valid = true;
            for (const auto& bathroom : bathrooms) {
                if (calculateDistance(center, bathroom) < minDistance) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                suggestedBathrooms.push_back(center);
                bathrooms.push_back(center);
            }
        }

        return suggestedBathrooms;
    }

public:
    // Constructor
    BathroomOptimizer(const std::vector<Location>& spots, double minDist = 10.0)
        : hotspots(spots), minDistance(minDist) {}

    // Method to add existing bathrooms
    void addExistingBathrooms(const std::vector<Location>& existing) {
        bathrooms = existing;
    }

    // Method to evaluate coverage percentage
    double evaluateCoverage(const std::vector<Location>& suggestedBathrooms, double maxDistance) {
        int covered = 0;
        for (const auto& hotspot : hotspots) {
            for (const auto& bathroom : suggestedBathrooms) {
                if (calculateDistance(hotspot, bathroom) <= maxDistance) {
                    covered++;
                    break;
                }
            }
        }
        return (static_cast<double>(covered) / hotspots.size()) * 100.0;
    }

    // Get existing bathrooms
    std::vector<Location> getBathrooms() const {
        return bathrooms;
    }

    // Greedy algorithm interface
    std::vector<Location> runGreedy(int numToilets, double maxDistance) {
        return greedyPlacement(numToilets, maxDistance);
    }

    // K-Means algorithm interface
    std::vector<Location> runKMeans(int numToilets, double maxDistance) {
        return kMeansClustering(numToilets, maxDistance);
    }
};

// Function to generate random hotspots
std::vector<Location> generateRandomHotspots(int num, double maxX, double maxY, int maxTraffic) {
    std::vector<Location> hotspots;
    for (int i = 0; i < num; ++i) {
        Location loc;
        loc.x = static_cast<double>(rand()) / RAND_MAX * maxX;
        loc.y = static_cast<double>(rand()) / RAND_MAX * maxY;
        loc.traffic = rand() % maxTraffic + 1;  // Traffic between 1 and maxTraffic
        hotspots.push_back(loc);
    }
    return hotspots;
}

// Function to save bathroom locations to CSV
void saveToCSV(const std::string& filename, const std::vector<Location>& bathrooms) {
    std::ofstream outFile(filename);
    outFile << "Bathroom_X,Bathroom_Y,Traffic\n";
    for (const auto& bathroom : bathrooms) {
        outFile << bathroom.x << "," << bathroom.y << "," << bathroom.traffic << "\n";
    }
    outFile.close();
}

// Main function
int main() {
    srand(static_cast<unsigned>(time(0)));

    // Parameters
    int numHotspots = 100;        // Number of tourist hotspots
    double areaSize = 100.0;      // Size of the area (100x100 grid)
    int maxTraffic = 200;         // Maximum traffic per hotspot
    std::vector<Location> existingBathrooms = {
        {10, 10, 0},
        {50, 50, 0},
        {70, 80, 0}
    };
    int numToilets = 10;          // Number of toilets to place
    double maxDistance = 15.0;    // Maximum walking distance for coverage

    // Generate random hotspots
    std::vector<Location> hotspots = generateRandomHotspots(numHotspots, areaSize, areaSize, maxTraffic);

    // Initialize BathroomOptimizer
    BathroomOptimizer optimizer(hotspots);
    optimizer.addExistingBathrooms(existingBathrooms);

    // Greedy Algorithm
    auto startGreedy = std::chrono::high_resolution_clock::now();
    std::vector<Location> greedyBathrooms = optimizer.runGreedy(numToilets, maxDistance);
    auto endGreedy = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedGreedy = endGreedy - startGreedy;
    double greedyCoverage = optimizer.evaluateCoverage(greedyBathrooms, maxDistance);

    // Reset bathrooms for K-Means
    optimizer.addExistingBathrooms(existingBathrooms);

    // K-Means Algorithm
    auto startKMeans = std::chrono::high_resolution_clock::now();
    std::vector<Location> kMeansBathrooms = optimizer.runKMeans(numToilets, maxDistance);
    auto endKMeans = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedKMeans = endKMeans - startKMeans;
    double kMeansCoverage = optimizer.evaluateCoverage(kMeansBathrooms, maxDistance);

    // Output Results
    std::cout << "=== Greedy Algorithm ===\n";
    std::cout << "Suggested locations for new bathrooms:\n";
    for (const auto& bathroom : greedyBathrooms) {
        std::cout << "(" << bathroom.x << ", " << bathroom.y << ") with traffic: " << bathroom.traffic << "\n";
    }
    std::cout << "Coverage: " << greedyCoverage << "%\n";
    std::cout << "Execution Time: " << elapsedGreedy.count() << " seconds.\n\n";

    std::cout << "=== K-Means Clustering Algorithm ===\n";
    std::cout << "Suggested locations for new bathrooms:\n";
    for (const auto& bathroom : kMeansBathrooms) {
        std::cout << "(" << bathroom.x << ", " << bathroom.y << ") with traffic: " << bathroom.traffic << "\n";
    }
    std::cout << "Coverage: " << kMeansCoverage << "%\n";
    std::cout << "Execution Time: " << elapsedKMeans.count() << " seconds.\n\n";

    // Save results to CSV for visualization
    saveToCSV("greedy_bathrooms.csv", greedyBathrooms);
    saveToCSV("kmeans_bathrooms.csv", kMeansBathrooms);

    std::cout << "Results have been saved to 'greedy_bathrooms.csv' and 'kmeans_bathrooms.csv'.\n";

    return 0;
}
