
#include <windows.h>
#include <filesystem>

#include "safetyhook.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

// Struct to hold log parameters
struct MugenLogParams
{
    std::string round;
    std::string gametime;
    std::string slot_id;
    std::string loops;
    std::string statedef;
    std::string state_no;
    std::string state_type;
    std::string clipboard;
};

// Helper functions to read game data
void* get_ptr(void* ptr) {
    return *(void**)(ptr);
}
void* get_gamevar() {
    return get_ptr((void*)0x4b5b4c);
}
void* get_player(uint32_t slot_id) {
	void* g = get_gamevar();
	if (g == nullptr) return nullptr;

	void* player = get_ptr((void*)((uint32_t)g + 0xb754 + slot_id * 0x4));
	return player;
}
void* get_player_cache(uint32_t slot_id) {
    void* player = get_player(slot_id);
	if (player == nullptr) return nullptr;

    void* player_cache = get_ptr((void*)((uint32_t)player));
    return player_cache;
}
std::string get_player_display_name(uint32_t slot_id) {
	void* player_cache = get_player_cache(slot_id);
	if (player_cache == nullptr) return "";
	char name[0x30];
	memcpy(name, (void*)((uint32_t)player_cache + 0x30), 0x30);
	name[0x2F] = '\0';
	return std::string(name);
}
std::string get_array_cur_idx(void** arr) {
	if (arr == nullptr) return "";
	int32_t cur_idx = *(int32_t*)((uint32_t)*arr + 0x1C);
	return std::to_string(cur_idx);
}

std::string get_state_type(uint32_t state_type_id) {
    switch (state_type_id) {
    case 0: return "Null";
    case 1: return "ChangeState";
    case 2: return "SelfState";
    case 3: return "Turn";
    case 4: return "VarSet";
    case 5: return "VarAdd";
    case 6: return "VarRandom";
    case 7: return "VarRangeSet";
    case 10: return "LifeSet";
    case 11: return "LifeAdd";
    case 12: return "PowerSet";
    case 13: return "PowerAdd";
    case 20: return "CtrlSet";
    case 21: return "PosSet";
    case 22: return "PosAdd";
    case 23: return "PosFreeze";
    case 24: return "VelSet";
    case 25: return "VelAdd";
    case 26: return "VelMul";
    case 27: return "Gravity";
    case 28: return "Offset";
    case 30: return "HitBy/NotHitBy";
    case 32: return "ChangeAnim";
    case 33: return "ChangeAnim2";
    case 34: return "PlaySnd";
    case 35: return "StopSnd";
    case 36: return "SndPan";
    case 37: return "HitDef";
    case 38: return "ReversalDef";
    case 39: return "Projectile";
    case 40: return "Width";
    case 50: return "ScreenBound";
    case 51: return "Sprpriority";
    case 60: return "AngleDraw";
    case 61: return "AngleSet";
    case 62: return "AngleAdd";
    case 63: return "AngleMul";
    case 65: return "Trans";
    case 70: return "TargetBind";
    case 71: return "BindToTarget";
    case 72: return "TargetState";
    case 73: return "TargetPowerAdd";
    case 74: return "TargetLifeAdd";
    case 75: return "TargetVelSet";
    case 76: return "TargetVelAdd";
    case 77: return "TargetFacing";
    case 78: return "TargetDrop";
    case 90: return "AttackDist";
    case 91: return "PlayerPush";
    case 100: return "HitFallSet";
    case 101: return "HitVelSet";
    case 102: return "HitFallDamage";
    case 103: return "HitFallVel";
    case 104: return "HitAdd";
    case 105: return "AttaclMulSet";
    case 106: return "DefenceMulSet";
    case 110: return "StateTypeSet";
    case 120: return "HitOverRide";
    case 130: return "MoveHitReset";
    case 150: return "AssertSpecial";
    case 200: return "EnvShake";
    case 201: return "FallEnvShake";
    case 202: return "EnvColor";
    case 203: return "GameMakeAnim";
    case 210: return "AfterImage";
    case 211: return "AfterImageTime";
    case 212: return "Pause";
    case 214: return "SuperPause";
    case 215: return "MakeDust";
    case 216: return "PalFx/BGPalFx/AllPalFx";
    case 220: return "Explod";
    case 221: return "RemoveExplod";
    case 222: return "ExplodBindTime";
    case 223: return "ModifyExplod";
    case 300: return "Helper";
    case 301: return "DestroySelf";
    case 304: return "ParentVarSet";
    case 305: return "ParentVarAdd";
    case 310: return "DisplayToClipboard";
    case 311: return "AppendToClipboard";
    case 312: return "ClearClipboard";
    case 320: return "BindToParent";
    case 321: return "BindToRoot";
    default: return "Unknown";
    }
}

// Singleton logger class
class StepLogger
{
public:
    static StepLogger& instance() {
        static StepLogger inst;
        return inst;
    }
    StepLogger(const StepLogger&) = delete;
    StepLogger& operator=(const StepLogger&) = delete;

	void set_round(const std::string& round) { params_.round = round; }
	void set_gametime(const std::string& gametime) { params_.gametime = gametime; }
	void set_slot_id(const std::string& slot_id) { params_.slot_id = slot_id; }
	void set_loops(const std::string& loops) { params_.loops = loops; }
	void set_statedef(const std::string& statedef) { params_.statedef = statedef; }
	void set_state_no(const std::string& state_no) { params_.state_no = state_no; }
	void set_state_type(const std::string& state_type) { params_.state_type = state_type; }
	void set_clipboard(const std::string& clipboard) { params_.clipboard = clipboard; }

	void write_params() {
        logger_->info("{}\t{}\t{}\t{}\t{}\t{}\t{}",
            params_.round, params_.gametime, params_.slot_id, params_.loops, params_.statedef, params_.state_no, params_.state_type);
        logger_->flush();
	}

    void init() {
        spdlog::drop(name_);
        auto dir = std::filesystem::path(getExeDir()) / "logs";
        std::filesystem::create_directories(dir);
        auto path = dir / ("mugenw_ex_" + currentDatetimeString() + ".log");
        logger_ = spdlog::basic_logger_mt(name_, path.string());
        logger_->set_level(spdlog::level::info);
        logger_->set_pattern("%v");

        params_ = {};
        set_round("Round");
        set_gametime("Gametime");
        set_slot_id("SlotID");
        set_loops("Loops");
        set_statedef("Statedef");
        set_state_no("StateNo");
        set_state_type("StateType");
    }

    template<typename... Args>
    void write(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        logger_->info(fmt, std::forward<Args>(args)...);
    }
private:
    StepLogger() {}
    ~StepLogger() {}

    std::string getExeDir() {
        char buf[MAX_PATH];
        GetModuleFileNameA(nullptr, buf, MAX_PATH);
        return std::filesystem::path(buf).parent_path().string();
    }
    std::string currentDatetimeString() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
        localtime_s(&tm, &t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
        return oss.str();
    }

    std::string name_ = "step_logger";
    MugenLogParams params_;
    std::shared_ptr<spdlog::logger>  logger_;
};

// Hooks
SafetyHookMid match_start_hook;
void match_start_hook_proc(SafetyHookContext& ctx) {
    StepLogger::instance().init();
	StepLogger::instance().write("[Match Start]");
	StepLogger::instance().write("Players:\nSlot0={}\nSlot1={}\nSlot2={}\nSlot3={}\n",
		get_player_display_name(0), get_player_display_name(1), get_player_display_name(2), get_player_display_name(3));
	StepLogger::instance().write_params();

	StepLogger::instance().set_loops("0");
}

SafetyHookMid get_state_type_hook;
void get_state_type_hook_proc(SafetyHookContext& ctx) {
	StepLogger::instance().set_state_type(get_state_type((uint32_t)ctx.eax));
    StepLogger::instance().write_params();
}

SafetyHookMid get_state_no_hook_01;
void get_state_no_hook_01_proc(SafetyHookContext& ctx) {
    StepLogger::instance().set_state_no(get_array_cur_idx((void**)ctx.ebx));
}
SafetyHookMid get_state_no_hook_02;
void get_state_no_hook_02_proc(SafetyHookContext& ctx) {
    StepLogger::instance().set_state_no(get_array_cur_idx((void**)ctx.ebp));
}

SafetyHookMid get_state_def_hook_01;
void get_state_def_hook_01_proc(SafetyHookContext& ctx) {
    StepLogger::instance().set_statedef(std::to_string((int32_t)ctx.edx));
}
SafetyHookMid get_state_def_hook_02;
void get_state_def_hook_02_proc(SafetyHookContext& ctx) {
    StepLogger::instance().set_statedef(std::to_string((int32_t)ctx.ecx));
}

SafetyHookMid get_loops_hook;
void get_loops_hook_proc(SafetyHookContext& ctx) {
    StepLogger::instance().set_loops(std::to_string((int32_t)ctx.eax));
}
SafetyHookMid reset_loops_hook;
void reset_loops_hook_proc(SafetyHookContext& ctx) {
    StepLogger::instance().set_loops("0");
}

SafetyHookMid get_slot_id_hook_01, get_slot_id_hook_02, get_slot_id_hook_03;
void get_slot_id_hook_proc(SafetyHookContext& ctx) {
	StepLogger::instance().set_slot_id(std::to_string((int32_t)ctx.ebx));
}

SafetyHookMid get_gametime_hook;
void get_gametime_hook_proc(SafetyHookContext& ctx) {
	StepLogger::instance().set_gametime(std::to_string(*(int32_t*)((int32_t)ctx.eax + 0xb3fc)));
    StepLogger::instance().set_round(std::to_string(*(int32_t*)((int32_t)ctx.eax + 0xbc04)));
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        match_start_hook = safetyhook::create_mid((void*)0x42e13f, match_start_hook_proc); // GameMatchReset
		get_state_type_hook = safetyhook::create_mid((void*)0x46e830, get_state_type_hook_proc); // PlayerSCtrlApplyElem

        get_state_no_hook_01 = safetyhook::create_mid((void*)0x47f882, get_state_no_hook_01_proc); // PlayerSCtrlUpdateSpec
        get_state_no_hook_02 = safetyhook::create_mid((void*)0x47f775, get_state_no_hook_02_proc); // PlayerSCtrlUpdate

		get_state_def_hook_01 = safetyhook::create_mid((void*)0x47f82b, get_state_def_hook_01_proc); // PlayerSCtrlUpdateSpec
		get_state_def_hook_02 = safetyhook::create_mid((void*)0x47f41f, get_state_def_hook_02_proc); // PlayerSCtrlUpdate

        get_loops_hook = safetyhook::create_mid((void*)0x44152c, get_loops_hook_proc); // PlayerUpdate
		reset_loops_hook = safetyhook::create_mid((void*)0x441575, reset_loops_hook_proc); // PlayerUpdate

		get_slot_id_hook_01 = safetyhook::create_mid((void*)0x41f57d, get_slot_id_hook_proc); // GameRound
        get_slot_id_hook_02 = safetyhook::create_mid((void*)0x41f5dd, get_slot_id_hook_proc); // GameRound
        get_slot_id_hook_03 = safetyhook::create_mid((void*)0x41f65b, get_slot_id_hook_proc); // GameRound

		get_gametime_hook = safetyhook::create_mid((void*)0x41d216, get_gametime_hook_proc); // GameLoop
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}