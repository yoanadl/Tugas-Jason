#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cctype>
#include <map>
using namespace std;

// ---------------
// City Structure
// ---------------
struct City {
    int x, y;
    int id;
    string name;
};

// Variables for grid dimensions
int gridX_min = 0, gridX_max = 8, gridY_min = 0, gridY_max = 8;
int grid_width = 9, grid_height = 9;

// Dynamic 2D arrays for different data types
int** cityGrid = nullptr;
int** cloudData = nullptr;
int** pressureData = nullptr;

// File names read from configuration file
string cityFileName = "";
string cloudFileName = "";
string pressureFileName = "";

// To store all city information and to check if config has been loaded
vector<City> cities;
bool configLoaded = false;

// ----------------------
// Function Declarations
// ----------------------
bool readCityData();
bool readCloudData();
bool readPressureData();
int getValidChoice();
void waitForEnter();

// --------------------------
// Memory Management Function 
// --------------------------
void allocateGrids() {
    // Clean up existing grids first
    if (cityGrid) {
        for (int i = 0; i < grid_height; i++) {
            delete[] cityGrid[i]; // Delete each row
        }
        delete[] cityGrid; // Delete the array of pointers
    }
    if (cloudData) {
        for (int i = 0; i < grid_height; i++) {
            delete[] cloudData[i];
        }
        delete[] cloudData;
    }
    if (pressureData) {
        for (int i = 0; i < grid_height; i++) {
            delete[] pressureData[i];
        }
        delete[] pressureData;
    }
    
    // Calculate new grid dimensions (total columns & rows)
    grid_width = gridX_max - gridX_min + 1; 
    grid_height = gridY_max - gridY_min + 1;  
    
    // Allocate city grid (store city IDs)
    cityGrid = new int*[grid_height];
    for (int i = 0; i < grid_height; i++) {
        cityGrid[i] = new int[grid_width];
        for (int j = 0; j < grid_width; j++) {
            cityGrid[i][j] = 0; // 0 means no city
        }
    }
    
    // Allocate cloud data grid (store cloud values 0-99)
    cloudData = new int*[grid_height];
    for (int i = 0; i < grid_height; i++) {
        cloudData[i] = new int[grid_width];
        for (int j = 0; j < grid_width; j++) {
            cloudData[i][j] = 0;
        }
    }
    
    // Allocate pressure data grid (store pressure values 0-99)
    pressureData = new int*[grid_height];
    for (int i = 0; i < grid_height; i++) {
        pressureData[i] = new int[grid_width];
        for (int j = 0; j < grid_width; j++) {
            pressureData[i][j] = 0;
        }
    }
}

// To free all allocated memory, called at program exit to prevent memory leaks
void deallocateGrids() {
    if (cityGrid) {
        for (int i = 0; i < grid_height; i++) {
            delete[] cityGrid[i]; // Delete each row
        }
        delete[] cityGrid; // Delete array of pointers
        cityGrid = nullptr; // Set pointer to null
    }
    
    if (cloudData) {
        for (int i = 0; i < grid_height; i++) {
            delete[] cloudData[i];
        }
        delete[] cloudData;
        cloudData = nullptr;
    }
    
    if (pressureData) {
        for (int i = 0; i < grid_height; i++) {
            delete[] pressureData[i];
        }
        delete[] pressureData;
        pressureData = nullptr;
    }
}

// -------------
// Trim Function
// -------------
// Remove whitespace from beginning and end of string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// --------------------------
// Configuration File Reading
// --------------------------
// Encourage user to put in filename and read the file line by line
void readConfigFile() {
    cout << "Please enter config filename : ";
    string filename;
    getline(cin, filename); // Read entire line including spaces
    
    filename = trim(filename); // Trim extra whitespaces
    
    ifstream file(filename); // Open file for reading
    if (!file) { // Check if successful or not
        cout << "Error: Cannot open " << filename << endl;
        waitForEnter();
        return;
    }
    
    string line;
    cout << "\nReading config file..." << endl;
   
   // Read file line by line 
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        
        cout << line << endl; 
        
        // Parse Grid X Range
        if (line.find("GridX_IdxRange") != string::npos) {
            size_t colonPos = line.find(':');
            if (colonPos != string::npos) {
                string rangeStr = trim(line.substr(colonPos + 1)); // Get part after ::
                size_t dashPos = rangeStr.find('-');
                if (dashPos != string::npos) {
                    try {
                        string minStr = trim(rangeStr.substr(0, dashPos));
                        string maxStr = trim(rangeStr.substr(dashPos + 1));
                        gridX_min = stoi(minStr); // Convert to integer
                        gridX_max = stoi(maxStr);
                    } catch (const exception& e) {
                        cout << "Warning: Could not parse GridX_IdxRange" << endl;
                    }
                }
            }
        }
        // Parge Grid Y Range
        else if (line.find("GridY_IdxRange") != string::npos) {
            size_t colonPos = line.find(':');
            if (colonPos != string::npos) {
                string rangeStr = trim(line.substr(colonPos + 1));
                size_t dashPos = rangeStr.find('-');
                if (dashPos != string::npos) {
                    try {
                        string minStr = trim(rangeStr.substr(0, dashPos));
                        string maxStr = trim(rangeStr.substr(dashPos + 1));
                        gridY_min = stoi(minStr);
                        gridY_max = stoi(maxStr);
                    } catch (const exception& e) {
                        cout << "Warning: Could not parse GridY_IdxRange" << endl;
                    }
                }
            }
        }
        // Identify Data Files
        else if (line.find("CityLocation.txt") != string::npos) {
            cityFileName = trim(line);  
        }
        else if (line.find("CloudCover.txt") != string::npos) {
            cloudFileName = trim(line);  
        }
        else if (line.find("Pressure.txt") != string::npos) {
            pressureFileName = trim(line); 
        }
    }
    file.close();
    
    // Allocate memory grids based on parsed dimensions
    allocateGrids();
    configLoaded = true; // mark config as loaded
    
    // Display sumary of what file was loaded
    cout << "\nConfiguration loaded successfully!" << endl;
    cout << "Grid dimensions: [" << gridX_min << "-" << gridX_max << "] x [" << gridY_min << "-" << gridY_max << "]" << endl;
    if (!cityFileName.empty()) cout << "City file: " << cityFileName << endl;
    if (!cloudFileName.empty()) cout << "Cloud file: " << cloudFileName << endl;  
    if (!pressureFileName.empty()) cout << "Pressure file: " << pressureFileName << endl;
    
    waitForEnter(); 
}

// --------------------------
// Data File Reading Function
// --------------------------
bool readCityData() {
    if (cityFileName.empty()) {
        cout << "Error: City filename not found. Please read config file first!" << endl;
        return false;
    }
    
    ifstream file(cityFileName);
    if (!file) {
        cout << "Error: Cannot open " << cityFileName << endl;
        return false;
    }
    
    cities.clear(); // clear existing city if any
    
    // Clear city grid
    for (int i = 0; i < grid_height; i++) {
        for (int j = 0; j < grid_width; j++) {
            cityGrid[i][j] = 0;
        }
    }
    
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        
        try {
            City city;
            // Parse format: [x, y]-id-Name
            size_t start = line.find('[');
            size_t comma = line.find(',');
            size_t end = line.find(']');
            
            if (start == string::npos || comma == string::npos || end == string::npos) {
                continue; // Skip malformed lines
            }
            
            // Extract X & Y Coordinates
            city.x = stoi(trim(line.substr(start + 1, comma - start - 1)));
            city.y = stoi(trim(line.substr(comma + 1, end - comma - 1)));
            
            // Find the parts after ]
            string remaining = line.substr(end + 1);
            size_t dash1 = remaining.find('-');
            size_t dash2 = remaining.find('-', dash1 + 1);
            
            if (dash1 != string::npos && dash2 != string::npos) {
                city.id = stoi(trim(remaining.substr(dash1 + 1, dash2 - dash1 - 1)));
                city.name = trim(remaining.substr(dash2 + 1));
            }
            
            cities.push_back(city); // Add city to vector
            
            // Mark city in grid
            int gridX = city.x - gridX_min; // Convert world coords to grid coord
            int gridY = city.y - gridY_min;
            if (gridX >= 0 && gridX < grid_width && gridY >= 0 && gridY < grid_height) {
                cityGrid[gridY][gridX] = city.id; // Store city ID at this position
            }
        } catch (const exception& e) {
            cout << "Warning: Could not parse line: " << line << endl;
        }
    }
    file.close();
    return true;
}

bool readCloudData() {
    if (cloudFileName.empty()) {
        cout << "Error: Cloud filename not found. Please read config file first!" << endl;
        return false;
    }
    
    ifstream file(cloudFileName);
    if (!file) {
        cout << "Error: Cannot open " << cloudFileName << endl;
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        
        try {
            // Parse format: [x, y]-value
            size_t start = line.find('[');
            size_t comma = line.find(',');
            size_t end = line.find(']');
            size_t dash = line.find('-', end); 
            
            if (start == string::npos || comma == string::npos || 
                end == string::npos || dash == string::npos) {
                continue; // Skip malformed lines
            }
            
            // Extract coordinates and value
            int x = stoi(trim(line.substr(start + 1, comma - start - 1)));
            int y = stoi(trim(line.substr(comma + 1, end - comma - 1)));
            int value = stoi(trim(line.substr(dash + 1)));
            
            // Store in grid
            int gridX = x - gridX_min;
            int gridY = y - gridY_min;
            if (gridX >= 0 && gridX < grid_width && gridY >= 0 && gridY < grid_height) {
                cloudData[gridY][gridX] = value;
            }
        } catch (const exception& e) {
            cout << "Warning: Could not parse cloud data line: " << line << endl;
        }
    }
    file.close();
    return true;
}

// ---------------------------
// Read Pressure Data Function
// ---------------------------
bool readPressureData() {
    if (pressureFileName.empty()) {
        cout << "Error: Pressure filename not found. Please read config file first!" << endl;
        return false;
    }
    
    ifstream file(pressureFileName);
    if (!file) {
        cout << "Error: Cannot open " << pressureFileName << endl;
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        
        try {
            // Parse format: [x, y]-value
            size_t start = line.find('[');
            size_t comma = line.find(',');
            size_t end = line.find(']');
            size_t dash = line.find('-', end);
            
            if (start == string::npos || comma == string::npos || 
                end == string::npos || dash == string::npos) {
                continue; // Skip malformed lines
            }
            
            // Extract coordinates and values
            int x = stoi(trim(line.substr(start + 1, comma - start - 1)));
            int y = stoi(trim(line.substr(comma + 1, end - comma - 1)));
            int value = stoi(trim(line.substr(dash + 1)));
            
            // Store in grid
            int gridX = x - gridX_min;
            int gridY = y - gridY_min;
            if (gridX >= 0 && gridX < grid_width && gridY >= 0 && gridY < grid_height) {
                pressureData[gridY][gridX] = value;
            }
        } catch (const exception& e) {
            cout << "Warning: Could not parse pressure data line: " << line << endl;
        }
    }
    file.close();
    return true;
}

// -------------------------
// Display City Map Function
// -------------------------
void displayCityMap() {
    if (!configLoaded) {
        cout << "Please read config file first!" << endl;
        waitForEnter();
        return;
    }
    
    // Load city data
    if (!readCityData()) {
        waitForEnter();
        return;
    }
    
    cout << "\nCity Map" << endl;
    cout << "--------" << endl;
    
    // Print top border
    cout << "     ";                                      // Space for y-axis labels
    for (int x = gridX_min - 1; x <= gridX_max; x++) {   
        cout << "# ";                                     // Each '#' with space
    }
    cout << "#" << endl;                                  // Final '#' for right border
    
    // Print grid with Y-axis from top to bottom (gridY_max to gridY_min)
    for (int y = gridY_max; y >= gridY_min; y--) {
        // Print Y-axis label and left border
        cout << setw(3) << y << "  # ";
        
        // Print grid content with spaces
        for (int x = gridX_min; x <= gridX_max; x++) {
            int gridX = x - gridX_min;
            int gridY = y - gridY_min;
            if (cityGrid[gridY][gridX] != 0) {
                cout << cityGrid[gridY][gridX] << " ";
            } else {
                cout << "  ";
            }
        }
        
        // Print right border
        cout << "#" << endl;
    }
    
    // Print bottom border
    cout << "     ";
    for (int x = gridX_min - 1; x <= gridX_max; x++) {
        cout << "# ";
    }
    cout << "#" << endl;
    
    // Print X-axis labels at the bottom
    cout << "       ";
    for (int x = gridX_min; x <= gridX_max; x++) {
        cout << x << " ";
    }
    cout << endl;
    
    waitForEnter();
}

// ----------------------------
// Display Cloud Coverage Index
// ----------------------------
// Shows cloud coverage as index values
void displayCloudCoverageIndex() {
    if (!configLoaded) {
        cout << "Please read config file first!" << endl;
        waitForEnter();
        return;
    }
    
    // Load cloud data
    if (!readCloudData()) {
        waitForEnter();
        return;
    }
    
    cout << "\nCloud Coverage Map (Cloudiness Index)" << endl;
    cout << "-------------------------------------" << endl;
    
    // Print top border
    cout << "     ";
    for (int x = gridX_min - 1; x <= gridX_max; x++) {
        cout << "# ";
    }
    cout << "#" << endl;
    
    // Print grid with cloudiness index
    for (int y = gridY_max; y >= gridY_min; y--) {
        cout << setw(3) << y << "  # ";
        for (int x = gridX_min; x <= gridX_max; x++) {
            int gridX = x - gridX_min;
            int gridY = y - gridY_min;
            int value = cloudData[gridY][gridX];
            
            // Convert to cloudiness index (0-9)
            int index = value / 10;
            if (index > 9) index = 9;
            
            cout << index << " ";
        }
        cout << "#" << endl;
    }
    
    // Print bottom border
    cout << "     ";
    for (int x = gridX_min - 1; x <= gridX_max; x++) {
        cout << "# ";
    }
    cout << "#" << endl;
    
    // Print X-axis labels at the bottom
    cout << "       ";
    for (int x = gridX_min; x <= gridX_max; x++) {
        cout << x << " ";
    }
    cout << endl;
    
    waitForEnter();
}

// -----------------------------------
// Display Cloud Coverage LMH Function
// -----------------------------------
// Shows cloud coverage as Low/Medium/High symbols
void displayCloudCoverageLMH() {
    if (!configLoaded) {
        cout << "Please read config file first!" << endl;
        waitForEnter();
        return;
    }
    
    // Load cloud data
    if (!readCloudData()) {
        waitForEnter();
        return;
    }
    
    cout << "\nCloud Coverage Map (LMH symbols)" << endl;
    cout << "---------------------------------" << endl;
    
    // Print top border
    cout << "     ";
    for (int x = gridX_min - 1; x <= gridX_max; x++) {
        cout << "# ";
    }
    cout << "#" << endl;
    
    // Print grid with LMH symbols
    for (int y = gridY_max; y >= gridY_min; y--) {
        cout << setw(3) << y << "  # ";
        for (int x = gridX_min; x <= gridX_max; x++) {
            int gridX = x - gridX_min;
            int gridY = y - gridY_min;
            int value = cloudData[gridY][gridX];
            
            // Convert to LMH symbols according to Appendix C
            char symbol;
            if (value >= 0 && value < 35) {
                symbol = 'L';
            } else if (value >= 35 && value < 65) {
                symbol = 'M';
            } else {
                symbol = 'H';
            }
            
            cout << symbol << " ";
        }
        cout << "#" << endl;
    }
    
    // Print bottom border
    cout << "     ";
    for (int x = gridX_min - 1; x <= gridX_max; x++) {
        cout << "# ";
    }
    cout << "#" << endl;
    
    // Print X-axis labels at the bottom
    cout << "       ";
    for (int x = gridX_min; x <= gridX_max; x++) {
        cout << x << " ";
    }
    cout << endl;
    
    waitForEnter();
}

// -------------------------------
// Display Pressure Index Function
// -------------------------------
// Shows atmospheric pressure as index values (0-9)
void displayPressureIndex() {
    if (!configLoaded) {
        cout << "Please read config file first!" << endl;
        waitForEnter();
        return;
    }
    
    // Load pressure data
    if (!readPressureData()) {
        waitForEnter();
        return;
    }
    
    cout << "\nAtmospheric Pressure Map (Pressure Index)" << endl;
    cout << "------------------------------------------" << endl;
    
    // Print top border
    cout << "     ";
    for (int x = gridX_min - 1; x <= gridX_max; x++) {
        cout << "# ";
    }
    cout << "#" << endl;
    
    // Print grid with pressure index
    for (int y = gridY_max; y >= gridY_min; y--) {
        cout << setw(3) << y << "  # ";
        for (int x = gridX_min; x <= gridX_max; x++) {
            int gridX = x - gridX_min;
            int gridY = y - gridY_min;
            int value = pressureData[gridY][gridX];
            
            // Convert to pressure index (0-9)
            int index = value / 10;
            if (index > 9) index = 9;
            
            cout << index << " ";
        }
        cout << "#" << endl;
    }
    
    // Print bottom border
    cout << "     ";
    for (int x = gridX_min - 1; x <= gridX_max; x++) {
        cout << "# ";
    }
    cout << "#" << endl;
    
    // Print X-axis labels at the bottom
    cout << "       ";
    for (int x = gridX_min; x <= gridX_max; x++) {
        cout << x << " ";
    }
    cout << endl;
    
    waitForEnter();
}

// -----------------------------
// Display Pressure LMH Function
// -----------------------------
// Shows atmospheric pressure as Low/Medium/High symbols
void displayPressureLMH() {
    if (!configLoaded) {
        cout << "Please read config file first!" << endl;
        waitForEnter();
        return;
    }
    
    // Load pressure data
    if (!readPressureData()) {
        waitForEnter();
        return;
    }
    
    cout << "\nAtmospheric Pressure Map (LMH symbols)" << endl;
    cout << "--------------------------------------" << endl;
    
    // Print top border
    cout << "     ";
    for (int x = gridX_min - 1; x <= gridX_max; x++) {
        cout << "# ";
    }
    cout << "#" << endl;
    
    // Print grid with LMH symbols
    for (int y = gridY_max; y >= gridY_min; y--) {
        cout << setw(3) << y << "  # ";
        for (int x = gridX_min; x <= gridX_max; x++) {
            int gridX = x - gridX_min;
            int gridY = y - gridY_min;
            int value = pressureData[gridY][gridX];
            
            // Convert to LMH symbols according to Appendix D
            char symbol;
            if (value >= 0 && value < 35) {
                symbol = 'L';
            } else if (value >= 35 && value < 65) {
                symbol = 'M';
            } else {
                symbol = 'H';
            }
            
            cout << symbol << " ";
        }
        cout << "#" << endl;
    }
    
    // Print bottom border
    cout << "     ";
    for (int x = gridX_min - 1; x <= gridX_max; x++) {
        cout << "# ";
    }
    cout << "#" << endl;
    
    // Print X-axis labels at the bottom
    cout << "       ";
    for (int x = gridX_min; x <= gridX_max; x++) {
        cout << x << " ";
    }
    cout << endl;
    
    waitForEnter();
}

// -------------------------------
// Display Weather Report Function
// -------------------------------
// Shows detailed weather forecast for each city
void displayWeatherReport() {
    if (!configLoaded) {
        cout << "Please read config file first!" << endl;
        waitForEnter();
        return;
    }
    
    // checking data integrity
    if (!cityGrid || !cloudData || !pressureData) {
        cout << "Error: Grid data not allocated!" << endl;
        waitForEnter();
        return;
    }
    
    // Load all data
    if (!readCityData() || !readCloudData() || !readPressureData()) {
        waitForEnter();
        return;
    }
    
    cout << "\nWeather Forecast Summary Report" << endl;
    cout << "===============================" << endl;
    
    // Group cities by name to handle multi-cell cities
    map<string, vector<City>> cityGroups;
    for (const City& city : cities) {
        cityGroups[city.name].push_back(city);
    }
    
    // Process each unique city
    for (const auto& cityGroup : cityGroups) {
        string cityName = cityGroup.first;
        vector<City> cityAreas = cityGroup.second;
        
        // Get city ID from first occurrence
        int cityID = cityAreas[0].id;
        
        // Find all city grid positions
        vector<pair<int, int>> cityPositions;
        for (const City& city : cityAreas) {
            cityPositions.push_back({city.x, city.y});
        }
        
        // Find surrounding (perimeter) areas - 8-directional neighbors
        vector<pair<int, int>> perimeterPositions;
        for (const auto& pos : cityPositions) {
            // Check 8-directional neighbors (including diagonal)
            int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
            int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
            
            for (int i = 0; i < 8; i++) {
                int nx = pos.first + dx[i];
                int ny = pos.second + dy[i];
                
                // Check if within grid bounds
                if (nx < gridX_min || nx > gridX_max || ny < gridY_min || ny > gridY_max) {
                    continue;
                }
                
                // Check if it's not a city position
                bool isCityPos = false;
                for (const auto& cityPos : cityPositions) {
                    if (cityPos.first == nx && cityPos.second == ny) {
                        isCityPos = true;
                        break;
                    }
                }
                if (isCityPos) continue;
                
                // Check if already added to perimeter (to avoid duplicates)
                bool alreadyAdded = false;
                for (size_t k = 0; k < perimeterPositions.size(); k++) {
                    if (perimeterPositions[k].first == nx && perimeterPositions[k].second == ny) {
                        alreadyAdded = true;
                        break;
                    }
                }
                if (!alreadyAdded) {
                    perimeterPositions.push_back({nx, ny});
                }
            }
        }
        
        // Calculate ACC (Average Cloud Cover) 
        double totalCloud = 0;
        int validCloudAreas = 0;
        
        // Sum cloud values from city areas
        for (const auto& pos : cityPositions) {
            int gridX = pos.first - gridX_min;
            int gridY = pos.second - gridY_min;
            
            // checking boundaries before accessing
            if (gridX >= 0 && gridX < grid_width && gridY >= 0 && gridY < grid_height) {
                totalCloud += cloudData[gridY][gridX];
                validCloudAreas++;
            }
        }
        
        // Sum cloud values from perimeter areas
        for (const auto& pos : perimeterPositions) {
            int gridX = pos.first - gridX_min;
            int gridY = pos.second - gridY_min;
            
            // checking boundaries before accessing
            if (gridX >= 0 && gridX < grid_width && gridY >= 0 && gridY < grid_height) {
                totalCloud += cloudData[gridY][gridX];
                validCloudAreas++;
            }
        }
        
        // checking div by 0
        double ACC = (validCloudAreas > 0) ? totalCloud / validCloudAreas : 0;
        
        // Calculate AP (Average Pressure) with 
        double totalPressure = 0;
        int validPressureAreas = 0;
        
        // Sum pressure values from city areas
        for (const auto& pos : cityPositions) {
            int gridX = pos.first - gridX_min;
            int gridY = pos.second - gridY_min;
            
            // checking boundaries before accessing
            if (gridX >= 0 && gridX < grid_width && gridY >= 0 && gridY < grid_height) {
                totalPressure += pressureData[gridY][gridX];
                validPressureAreas++;
            }
        }
        
        // Sum pressure values from perimeter areas
        for (const auto& pos : perimeterPositions) {
            int gridX = pos.first - gridX_min;
            int gridY = pos.second - gridY_min;
            
            // checking boundaries before accessing
            if (gridX >= 0 && gridX < grid_width && gridY >= 0 && gridY < grid_height) {
                totalPressure += pressureData[gridY][gridX];
                validPressureAreas++;
            }
        }
        
        // checking div by 0
        double AP = (validPressureAreas > 0) ? totalPressure / validPressureAreas : 0;
        
        // Determine LMH symbols for ACC and AP
        char accSymbol = (ACC < 35) ? 'L' : (ACC < 65) ? 'M' : 'H';
        char apSymbol = (AP < 35) ? 'L' : (AP < 65) ? 'M' : 'H';
        
        // Lookup probability of rain based on the table in Appendix E
        int rainProbability = 50; // Default
        
        if (accSymbol == 'H' && apSymbol == 'L') {
            rainProbability = 90;
        }
        else if (accSymbol == 'M' && apSymbol == 'L') {
            rainProbability = 80;
        }
        else if (accSymbol == 'L' && apSymbol == 'L') {
            rainProbability = 70;
        }
        else if (accSymbol == 'H' && apSymbol == 'M') {
            rainProbability = 60;
        }
        else if (accSymbol == 'M' && apSymbol == 'M') {
            rainProbability = 50;
        }
        else if (accSymbol == 'L' && apSymbol == 'M') {
            rainProbability = 40;
        }
        else if (accSymbol == 'H' && apSymbol == 'H') {
            rainProbability = 30;
        }
        else if (accSymbol == 'M' && apSymbol == 'H') {
            rainProbability = 20;
        }
        else if (accSymbol == 'L' && apSymbol == 'H') {
            rainProbability = 10;
        }
        
        // Display city report
        cout << "\nCity Name : " << cityName << endl;
        cout << "City ID : " << cityID << endl;
        cout << "Ave. Cloud Cover (ACC) : " << fixed << setprecision(2) << ACC << " (" << accSymbol << ")" << endl;
        cout << "Ave. Pressure (AP) : " << fixed << setprecision(2) << AP << " (" << apSymbol << ")" << endl;
        cout << "Probability of Rain (%) : " << fixed << setprecision(2) << (double)rainProbability << endl;
        
        // ASCII graphics from second code - but with safety
        if (rainProbability == 90) {
            cout << "" << endl;                 // 40% base
            cout << "~" << endl;                // Separator (5 tildes)
            cout << "\\\\\\\\\\" << endl;          // 50% extra (5 backslashes)
        }
        else if (rainProbability == 80) {
            cout << "" << endl;                 // 40% base
            cout << "~" << endl;                // Separator
            cout << "\\\\\\\\" << endl;            // 40% extra (4 backslashes)
        }
        else if (rainProbability == 70) {
            cout << "" << endl;                 // 40% base
            cout << "~" << endl;                // Separator
            cout << "\\\\\\" << endl;              // 30% extra (3 backslashes)
        }
        else if (rainProbability == 60) {
            cout << "" << endl;                 // 40% base
            cout << "~" << endl;                // Separator
            cout << "\\\\" << endl;                // 20% extra (2 backslashes)
        }
        else if (rainProbability == 50) {
            cout << "" << endl;                 // 40% base
            cout << "~" << endl;                // Separator
            cout << "\\" << endl;                  // 10% extra (1 backslash)
        }
        else if (rainProbability == 40) {
            cout << "" << endl;                 // 40% base
            cout << "~" << endl;                // Separator
            // No extra (40% total)
        }
        else if (rainProbability == 30) {
            cout << "~" << endl;                  // 30% total (3 tildes)
            cout << "" << endl;                 // Separator (base + 1)
        }
        else if (rainProbability == 20) {
            cout << "" << endl;                   // 20% total (2 tildes)
            cout << "~" << endl;                  // Separator (base + 1)
        }
        else if (rainProbability == 10) {
            cout << "~" << endl;                    // 10% total (1 tilde)
            cout << "" << endl;                   // Separator (base + 1)
        }
    }
    
    waitForEnter();
}

// ------------------------
// User Interface Functions
// ------------------------
// DIsplays the main menu with all available options
void showMenu() {
    cout << "Student ID: 8551285" << endl;
    cout << "Student Name: Jason Agnus Dei Liemanta" << endl;
    cout << "---------------------------------------------" << endl;
    cout << "Welcome to Weather Information Processing System!\n" << endl;
    cout << "1) Read in and process a configuration file" << endl;
    cout << "2) Display city map" << endl;
    cout << "3) Display cloud coverage map (cloudiness index)" << endl;
    cout << "4) Display cloud coverage map (LMH symbols)" << endl;
    cout << "5) Display atmospheric pressure map (pressure index)" << endl;
    cout << "6) Display atmospheric pressure map (LMH symbols)" << endl;
    cout << "7) Show weather forecast summary report" << endl;
    cout << "8) Quit" << endl;
    cout << "Please enter your choice : ";
}

// -----------------------
// Wait for enter function
// -----------------------
void waitForEnter() {
    cout << "\nPress <enter> to go back to main menu...";
    string dummy;                                        
    getline(cin, dummy);                            
}

// -------------------------
// Get Valid Choice Function
// -------------------------
int getValidChoice() {
    int choice;
    string input;
    
    // Loop until valid input
    while (true) {
        getline(cin, input);
        input = trim(input);
        
        // Check if input is empty
        if (input.empty()) {
            cout << "Please enter a valid choice (1-8): ";
            continue;
        }
        
        // Check if input contains only digits
        bool isValid = true;
        for (char c : input) {
            if (!isdigit(c)) {
                isValid = false;
                break;
            }
        }
        
        if (!isValid) {
            cout << "Invalid input! Please enter a number (1-8): ";
            continue;
        }
        
        // Convert string to integer
        try {
            choice = stoi(input);
        } catch (const exception& e) {
            cout << "Invalid input! Please enter a number (1-8): ";
            continue;
        }
        
        // Check if choice is in valid range
        if (choice >= 1 && choice <= 8) {
            return choice;
        } else {
            cout << "Please enter a valid choice (1-8): ";
        }
    }
}

// -------------
// Main Function
// -------------
int main() {
    int choice = 0;
    
    // Initialize default grids
    allocateGrids();
    
    // Main Program Loop
    do {
        showMenu();
        choice = getValidChoice();
        
        // Run Chosen Function
        switch (choice) {
            case 1:
                readConfigFile();
                break;
            case 2:
                displayCityMap();
                break;
            case 3:
                displayCloudCoverageIndex();
                break;
            case 4:
                displayCloudCoverageLMH();
                break;
            case 5:
                displayPressureIndex();
                break;
            case 6:
                displayPressureLMH();
                break;
            case 7:
                displayWeatherReport();
                break;
            case 8:
                cout << "Exiting Weather Information Processing System..." << endl;
                cout << "Thank you for using the program!" << endl;
                break;
        }
    } while (choice != 8);
    
    // Clean up dynamic memory
    deallocateGrids();
    
    return 0;
}