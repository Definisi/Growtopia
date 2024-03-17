#include <utils/klv_hash.hpp>

#include <array>

#include <utils/md5.hpp>
#include <utils/sha256.hpp>

/*
std::string LoginPacketGenerator::generate(const std::string& ubiticket)
{
    utils::TextParse var;
    //get_ticket();
    if (login_type == LOGIN_TYPE_UBISOFT) {
        var.add("UbiTicket", ubiticket);
    }
    else {
        if (m_id != "") //private server shit
            var.add("tankIDName", m_id);
        if (m_pass != "")//private server shit
            var.add("tankIDPass", m_pass);
    }
    var.add("requestedName", requestedname);
    var.add("f", "1");

    var.add("protocol", getProtocol(login_type));
    var.add("game_version", getVersion(login_type));//Better place????
    if (login_type == LOGIN_TYPE_WINDOWS or login_type == LOGIN_TYPE_UBISOFT) {
        var.add("fz", "38905384");
    }
    var.add("lmode", m_lmode);
    var.add("cbits", "1024");
    var.add("player_age", "25");
    var.add("GDPR", "1");
    var.add("category", "wotd_world");
    var.add("totalPlaytime", "0");
    if (login_type != LOGIN_TYPE_IOS) {
        var.add("klv", GenerateKLV(getProtocol(login_type), device_id_hash, getVersion(login_type), rid, login_type));
    }
    else {
        var.add("klv", "E8665123F164AC00CA367A27F299E1F3");
    }
    if (login_type == LOGIN_TYPE_WINDOWS or login_type == LOGIN_TYPE_UBISOFT or login_type == LOGIN_TYPE_MACOS) {
        var.add("hash2", utils::proton_hash(std::format("{}RT", mac).c_str()));
    }
    else if (login_type == LOGIN_TYPE_IOS) {
        var.add("hash2", "-305319022");
    }
    if (login_type == LOGIN_TYPE_IOS) {
        var.add("vid", Utils::generateRandomVid());
        var.add("aid", "");
    }
    if (login_type == LOGIN_TYPE_ANDROID) {
        var.add("gid", "");
        var.add("tr", "4322");
    }
    if (login_type == LOGIN_TYPE_MACOS) {
        var.add("tr", "5323");
    }


    var.add("meta", m_meta);
    var.add("fhash", "-716928004");
    if (login_type == LOGIN_TYPE_IOS) {
        var.add("rid", "01EB16DC6C7278FC0A248C8AC9DA1C80");
    }
    else {
        var.add("rid", rid);
    }
    if (login_type == LOGIN_TYPE_WINDOWS) {
        var.add("platformID", "0,1,1");
    }
    else if (login_type == LOGIN_TYPE_UBISOFT) {
        var.add("platformID", "13,1,1");
    }
    else if (login_type == LOGIN_TYPE_ANDROID) {
        var.add("platformID", "4");
    }
    else if (login_type == LOGIN_TYPE_IOS) {
        var.add("platformID", "1");
    }
    else if (login_type == LOGIN_TYPE_MACOS) {
        var.add("platformID", "3");
    }

    var.add("deviceVersion", "0");
    var.add("country", auth->bub_flag);
    if (login_type == LOGIN_TYPE_IOS) {
        var.add("hash", "-305319022");
    }
    else {
        var.add("hash", device_id_hash);
    }
    if (login_type == LOGIN_TYPE_WINDOWS or login_type == LOGIN_TYPE_UBISOFT or login_type == LOGIN_TYPE_MACOS) {
        var.add("mac", mac);
    }
    if (m_subserver)
    {
        var.add("user", m_user);
        var.add("token", m_token);
        var.add("UUIDToken", m_UUIDToken);
        var.add("doorID", m_doorID);
    }
    if (login_type == LOGIN_TYPE_ANDROID) {
        var.add("mac", mac);
    }
    if (login_type == LOGIN_TYPE_IOS) {
        var.add("mac", "02:00:00:00:00:00");
    }
    if (login_type == LOGIN_TYPE_WINDOWS or login_type == LOGIN_TYPE_UBISOFT) {
        var.add("wk", wk);
        var.add("zf", "677532082");
    }
    if (login_type == LOGIN_TYPE_ANDROID or login_type == LOGIN_TYPE_IOS) {
        var.add("wk", "NONE0");
    }
    if (login_type == LOGIN_TYPE_MACOS) {
        var.add("wk", wk);
    }
    std::string packet = var.get_all_raw();
    return packet;

}
*/

/*
constexpr uint32_t proton_hash(const char* data, std::size_t length = 0)
{
    uint32_t hash = 0x55555555;
    if (data) {
        if (length > 0) {
            while (length--)
                hash = (hash >> 27) + (hash << 5) + *reinterpret_cast<const uint8_t*>(data++);
        }
        else {
            while (*data)
                hash = (hash >> 27) + (hash << 5) + *reinterpret_cast<const uint8_t*>(data++);
        }
    }

    return hash;
}
*/


/* inline std::string generate_hex(T gen, std::size_t length, bool uppercase = true)
        {
            return generate(gen, length, uppercase
                ? "0123456789ABCDEF"
                : "0123456789abcdef");
        }

*
*  dmn
* ga mau utils/generator/ ? apa kek biar rapih
* // Itu pinmdahin dulu di biasanya utils
// zf gak berubah?
// zf dri kapan tau ga berubah berubah
// wk windows apa
// bentar mr hertzah lgi mengecek zf|1439718481 < ini yang di atas ganti
zf|677532082    
/*
* inline std::string generate_hex(T gen, std::size_t length, bool uppercase = true)
        {
            return generate(gen, length, uppercase
                ? "0123456789ABCDEF"
                : "0123456789abcdef");
        }
*  kan dah ada generatornya jir
*  iya itu buat contoh doang
* bentar afk di suruh anter kopi
utils::random::generate_hex(gen, 32, true);  wk
* // gw afk bentar ya buat aut h di docker agak ribet 
inline std::string generate_mac(T gen, bool zerotwo = true)
        {
            std::string result;
            result.reserve(17);

            if (zerotwo) {
                result.append("02");
                result.push_back(':');
            }

            for (std::size_t i = 0; i < (zerotwo ? 5 : 6); i++) {
                result.append(generate_hex(gen, 2, false));
                result.push_back(':');
            }

            result.pop_back();
            return result;
        }
*/

// Minta random mac sekalian codenya
// oke
namespace utils {
	std::string klv_hash(const float& game_version, const uint16_t& protocol, const std::string rid) {
		constexpr std::array salts = {
			"e9fc40ec08f9ea6393f59c65e37f750aacddf68490c4f92d0d2523a5bc02ea63",
			"c85df9056ee603b849a93e1ebab5dd5f66e1fb8b2f4a8caef8d13b9f9e013fa4",
			"3ca373dffbf463bb337e0fd768a2f395b8e417475438916506c721551f32038d",
			"73eff5914c61a20a71ada81a6fc7780700fb1c0285659b4899bc172a24c14fc1"
		};
		// wait
		// struktur login windows gimana. lupa yang ini android. yang old pula
		static std::array constant_values = {
			sha256(md5(sha256(std::to_string(protocol)))),
			sha256(sha256(std::to_string(game_version))),
			sha256(sha256(std::to_string(protocol)) + salts[3])
		};

		return sha256(constant_values[0]
			+ salts[0]
			+ constant_values[1]
			+ salts[1]
			+ sha256(md5(sha256(rid)))
			+ salts[2]
			+ constant_values[2]
		);
	}
}