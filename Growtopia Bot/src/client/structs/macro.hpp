struct Macro {
private:

public:
	bool auto_reconnect = true;

	bool auto_collect = false;
	bool auto_collect_force = false;
	int auto_collect_range = 0;
	int auto_collect_interval = 0;
	uint64_t auto_collect_last = 0;

	uint64_t smoke_last = 0; 
	int smoke_index = 0;

	std::map<std::pair<int, int>, bool> auto_farm_tile;
	bool auto_farm_place = false;
	bool auto_farm_punch = false;
	int auto_farm_index = 0;
	int auto_farm_block_id = 2;
	int auto_farm_delay_place = 200;
	int auto_farm_delay_punch = 180;
	uint64_t auto_farm_last = 0;

	std::vector<std::string> auto_spam_message;
	bool auto_spam = false;
	std::string auto_spam_world = "";
	std::string auto_spam_world_id = "";
	int auto_spam_delay = 4000;
	int auto_spam_x = 0;
	int auto_spam_y = 0;
};