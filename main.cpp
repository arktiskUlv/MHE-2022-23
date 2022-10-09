#include <fstream>
#include <iostream>
#include <string>
#include "json.hpp"

int main(int argc, char **argv) {
    using namespace std;
    try {
        string file_name = argv[1];
        ifstream file(file_name);
        nlohmann::json data = nlohmann::json::parse(file);

        string text = data["text"];
        int w = data["w"];
        int h = data["h"];

        int text_size = text.length();
        int w_size = w - text_size - 2;
        int l_space = ceil(w_size / 2.0);
        int r_space = floor(w_size / 2.0);
        int h_text_position = ceil(h / 2.0);

        int line_count = ceil(double(text_size) / double(w - 2));
        int char_count = ceil(double(text_size) / double(line_count));

        int total_space = (w - 2) * (h - 2);

        if (total_space < text_size) {
            cout << "given size will never be enough to contain all that text" << endl;
            return 1;
        }

        cout << string(w, '#') << endl;

        if (line_count <= 1) {
            for (int i = 0; i < h - 2; i++) {
                if (i == h_text_position - 2) {
                    cout << "#" << string(l_space, ' ') << text << string(r_space, ' ') << "#" << endl;
                } else {
                    cout << "#" << string(w - 2, ' ') << "#" << endl;
                }
            }
        } else {
            int top_lines = ceil(((h - 2) - line_count) / 2.0);
            int bottom_lines = floor(((h - 2) - line_count) / 2.0);
            int substring_iteration = 0;
            for (int i = 0; i < h - 2; i++) {
                if (i < top_lines or i > h - 2 - bottom_lines - 1) {
                    cout << "#" << string(w - 2, ' ') << "#" << endl;
                } else {
                    string my_str = text.substr(substring_iteration, char_count);
                    int number_of_spaces = (w - 2) - my_str.length();
                    int left_spaces_2 = ceil(double(number_of_spaces) / 2.0);
                    int right_spaces_2 = floor(double(number_of_spaces) / 2.0);
                    cout << "#" << string(left_spaces_2, ' ') << my_str << string(right_spaces_2, ' ') << "#" << endl;
                    substring_iteration += char_count;
                }
            }
        }
        cout << string(w, '#') << endl;
        return 0;
    }
    catch (...) {
        cout << "file must take one argument which is json file localization in shape "
                "{w:SZEROKOSCRAMKI,h:WYSOKOSCRAMKI,text:TEKSTDOWPISANIAWRAMCE}, SZEROKOSC"
                "  and WYSOKOSC must be an int and TEKST should be string" << endl;
        return 0;
    }
}