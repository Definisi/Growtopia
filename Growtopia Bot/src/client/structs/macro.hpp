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
	int auto_farm_block_id = 2;
	int auto_farm_delay_place = 200;
	int auto_farm_delay_punch = 180;
};