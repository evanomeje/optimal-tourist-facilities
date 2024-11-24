#include <vector>

// Basic structure to store locations
struct Location {
    double x;
    double y;
    int traffic;  // rough estimate of people per hour
};

// Very basic optimizer class - needs lots of work
class BathroomOptimizer {
private:
    std::vector<Location> hotspots;       // high traffic areas
    std::vector<Location> bathrooms;      // existing bathrooms
    
    // TODO: Add more parameters like:
    // - Construction costs
    // - Distance limits
    // - Traffic thresholds
    
    double getDistance(Location a, Location b) {
        // Very basic distance calculation
        // TODO: Implement proper distance formula
        return 0.0;
    }

public:
    BathroomOptimizer(std::vector<Location> spots) {
        hotspots = spots;
    }
    
    Location findBestSpot() {
        // INCOMPLETE: This is just a placeholder
        // Need to implement:
        // 1. Score calculation
        // 2. Traffic analysis
        // 3. Distance checking
        // 4. Cost considerations
        
        Location bestSpot = {0, 0, 0};  // dummy return
        
        // Rough idea of what we might do:
        for (auto spot : hotspots) {
            // Check if this spot is better than current best
            // TODO: Add actual comparison logic
        }
        
        return bestSpot;
    }
};

int main() {
    // Example usage - very basic
    std::vector<Location> resortSpots = {
        {10, 10, 100},  // restaurant
        {20, 20, 50},   // pool
    };
    
    BathroomOptimizer optimizer(resortSpots);
    Location suggestion = optimizer.findBestSpot();
    
    return 0;
}

//more stuff will be added