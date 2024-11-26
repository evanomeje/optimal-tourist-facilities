#include <vector>
#include <cmath>
#include <iostream>

// Basic structure to store locations
struct Location {
    double x;
    double y;
    int traffic;  // Rough estimate of people per hour
};

// Bathroom optimizer class
class BathroomOptimizer {
private:
    std::vector<Location> hotspots;       // High traffic areas
    std::vector<Location> bathrooms;      // Existing bathrooms

    // Calculate Euclidean distance
    double getDistance(Location a, Location b) {
        return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
    }

    // Check if the new bathroom is at a valid distance
    bool isValidDistance(Location newSpot) {
        for (const auto& bathroom : bathrooms) {
            if (getDistance(newSpot, bathroom) < 10.0) { // Minimum 10 units apart
                return false;
            }
        }
        return true;
    }

public:
    BathroomOptimizer(std::vector<Location> spots) {
        hotspots = spots;
    }

    void addExistingBathrooms(const std::vector<Location>& existing) {
        bathrooms = existing;
    }

    Location findBestSpot() {
        double bestScore = -1.0;
        Location bestSpot = {0, 0, 0};

        for (const auto& hotspot : hotspots) {
            double score = 0.0;

            // Evaluate score based on traffic and distance to existing bathrooms
            score += hotspot.traffic; // Higher traffic is better
            for (const auto& bathroom : bathrooms) {
                double dist = getDistance(hotspot, bathroom);
                if (dist < 10.0) {  // Penalize spots too close to existing bathrooms
                    score -= 50.0;
                }
            }

            // Update the best spot if this one is better
            if (score > bestScore && isValidDistance(hotspot)) {
                bestScore = score;
                bestSpot = hotspot;
            }
        }

        return bestSpot;
    }
};

int main() 
{
    // Example hotspots in the resort
    std::vector<Location> resortSpots = {
        {10, 10, 100},  // High traffic at restaurant
        {20, 20, 50},   // Medium traffic at pool
        {5, 25, 30},    // Low traffic at entrance
        {25, 5, 80}     // High traffic at souvenir shop
    };

    // Existing bathrooms
    std::vector<Location> existingBathrooms = {
        {5, 5, 0},   // Existing bathroom
        {15, 15, 0}  // Another existing bathroom
    };

    BathroomOptimizer optimizer(resortSpots);
    optimizer.addExistingBathrooms(existingBathrooms);

    // Find the best location for a new bathroom
    Location suggestion = optimizer.findBestSpot();

    std::cout << "Suggested location for new bathroom: (" 
              << suggestion.x << ", " << suggestion.y 
              << ") with traffic score: " << suggestion.traffic << std::endl;

    return 0;
}
