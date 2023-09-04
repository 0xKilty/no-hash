#include <iostream>
#include <unistd.h>
#include <functional>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <bitset>

using namespace std;

string get_next_name(string name) {
    if (name.substr(0, 10) != "./no-hash-") {
        cerr << "Executable is not formatted with no-hash-X exiting.\n";
    }
    size_t last_dash_index = name.find_last_of('-');
    int next_file_number = stoi(name.substr(last_dash_index + 1)) + 1;
    string next_name = name.substr(0, 10) + to_string(next_file_number);
    return next_name;
}

void copy_streams(ifstream &input, ofstream &output) {
    char buffer[1024];
    while (!input.eof()) {
        input.read(buffer, sizeof(buffer));
        streamsize bytes_read = input.gcount();
        output.write(buffer, bytes_read);
    }
}

bool is_printable(char c) {
    return c >= 32 && c <= 128;
}

uint64_t get_random_64_bits() {
    int *ptr = new int;
    delete ptr;
    int undefined_value = *ptr; 

    uint64_t time_with_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    int time_with_ms_int = static_cast<int>(time_with_ms & 0xFFFFFFFF); 

    uint64_t hashable_number;

    if (time_with_ms_int % 2 == 0) {
        hashable_number = time_with_ms_int;
        hashable_number <<= 32;
        hashable_number += undefined_value;
    } else {
        hashable_number = undefined_value;
        hashable_number <<= 32;
        hashable_number += time_with_ms_int;
    }

    hash<bitset<64> > bitset64_hasher;
    size_t hashed_number = bitset64_hasher(bitset<64>(hashable_number));

    return hashed_number;
}

void run_next_file(string next_file_name, int recursion_level) {
    cout << "Running command\n"; 
    system(("chmod +x " + next_file_name + " && " + next_file_name + " -r " + to_string(recursion_level)).c_str());
}

int main(int argc, char *argv[]) {
    cout << "\nRunning " << argv[0] << '\n';
    int recursive_level = 1;
    int option;
    while ((option = getopt(argc, argv, "r:")) != -1) {
        switch (option) {
        case 'r':
            recursive_level = atoi(optarg);
            if (recursive_level == 1) {
                cout << "Recursion depth hit, exiting.\n";
                return 0;
            }
            break;
        }
    }
    char myword[] = "Deez Nuts";
    string find_me = "Hello World";
    string find_me_also = "Here it is";

    cout << myword << '\n';

    ifstream this_file(argv[0], ios::binary);

    string file_name = argv[0];
    string next_name = get_next_name(file_name);
    ofstream next_file_output(next_name);

    int distance_count = 0;
    int count_amount = 0;

    char byte;
    string current_string;
    string temp_write = "New";

    int find_me_location;

    while (this_file.get(byte)) {
        if (is_printable(byte)) {
            current_string += byte;
        } else {
            if (!current_string.empty()) {
                if (current_string == find_me) {
                    cout << current_string << ' ' << this_file.tellg() << '\n';
                    for (size_t i = 0; i < current_string.size(); ++i) {
                        cout << bitset<8>(current_string.c_str()[i]) << ' ';
                    }
                    cout << '\n';
                    //byte = 'N';
                    count_amount = 1;
                } else if (current_string == find_me_also) {
                    cout << current_string << ' ' << this_file.tellg() << '\n';
                    for (size_t i = 0; i < current_string.size(); ++i) {
                        cout << bitset<8>(current_string.c_str()[i]) << ' ';
                    }
                    cout << '\n';
                    cout << distance_count << " bytes between\n";
                    count_amount = 0;
                }
                if (current_string == "Deez Nuts") {
                    cout << "Found " << current_string << " at " << this_file.tellg() << " sizeof = " << sizeof myword << '\n';
                }
                current_string.clear();
            }
        }
        distance_count += count_amount;
        next_file_output.put(byte);
    }

    next_file_output.close();
    this_file.close();

    run_next_file(next_name, recursive_level-1);

    return 0;
}