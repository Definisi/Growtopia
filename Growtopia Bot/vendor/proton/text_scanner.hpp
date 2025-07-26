#pragma once
#include <functional>
#include <sstream>
#include <string>
#include <vector>

class TextScanner {
private:
	std::vector<std::pair<std::string, std::vector<std::string>>> m_data;

	std::vector<std::pair<std::string, std::vector<std::string>>> parse(const std::string& string) {
		std::vector<std::pair<std::string, std::vector<std::string>>> data;
		std::istringstream stream(string);
		std::string line;

		while (std::getline(stream, line)) {
			std::istringstream line_stream(line);
			std::string label;
			std::getline(line_stream, label, '|');

			std::vector<std::string> values;
			std::string value;

			while (std::getline(line_stream, value, '|')) {
				if (label == "") {
					label = value;
					continue;
				}

				values.push_back(value);
			}

			data.emplace_back(label, values);
		}
		return data;
	}
public:
	TextScanner() = default;
	TextScanner(const std::string& string)
		: m_data(parse(string)) {}
	~TextScanner() = default;

    template<typename... Args>
    void add(const std::string& label, Args... values) {
        m_data.emplace_back(label, std::vector<std::string>{values...});
    }

    bool remove(const std::string& label) {
        auto it = std::find_if(m_data.begin(), m_data.end(), 
            [&](const std::pair<std::string, std::vector<std::string>>& pair) {
                return pair.first == label;
            });

        if (it != m_data.end()) {
            m_data.erase(it);
            return true;
        }
        return false;
    }

	void for_each(std::function<void(const std::string&, const std::vector<std::string>&)> callback) {
		for (auto pair : m_data) {
			callback(pair.first, pair.second);
		}
	}

	std::string get(const std::string& label, int index) const {
		auto it = std::find_if(m_data.begin(), m_data.end(), [&](const std::pair<std::string, std::vector<std::string>>& pair) {
			return pair.first == label;
			});

		if (it != m_data.end() && index >= 0 && index < it->second.size())
			return it->second[index];
		return "";
	}

	bool set(const std::string& label, int index, const std::string& value) {
		auto it = std::find_if(m_data.begin(), m_data.end(), [&](const auto& pair) {
			return pair.first == label;
			});

		if (it != m_data.end() && index >= 0 && index < it->second.size()) {
			it->second[index] = value;
			return true;
		}
		return false;
	}

	std::string get_all() const {
		std::ostringstream result;
		for (const auto& pair : m_data) {
			result << pair.first;
			if (pair.second.empty())
				result << '|';
			for (const auto& value : pair.second) {
				result << '|' << value;
			}
			result << '\n';
		}
		return result.str();
	}
};