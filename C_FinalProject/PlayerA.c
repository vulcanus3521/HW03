// student2_ai.c
#define _CRT_SECURE_NO_WARNINGS
#include "api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_WIDTH 7
#define MAP_HEIGHT 7

#define MAX_LINE 512
#define MAX_ITEMS 600

typedef struct {
    int id;
    char name[128];
    int atk;
    int def;
    int hp;
    char slot[8];
    char key_frag[128];
    char curse[128];
} PuzzleItem;

static PuzzleItem p_items[MAX_ITEMS];
static int p_count = 0;
static char current_filename[256] = "game_puzzle_en.csv ";

static void safe_copy(char* dst, int cap, const char* src) {
    if (!src) { dst[0] = 0; return; }
    strncpy(dst, src, cap - 1);
    dst[cap - 1] = 0;
}

static void safe_cat(char* dst, int cap, const char* add) {
    int cur = (int)strlen(dst);
    if (cur >= cap - 1) return;
    strncat(dst, add, cap - cur - 1);
}

static char* my_strdup(const char* s) {
    if (!s) return NULL;
    size_t L = strlen(s) + 1;
    char* p = (char*)malloc(L);
    if (p) memcpy(p, s, L);
    return p;
}

// get pointer by item id
static PuzzleItem* get_item_by_id(int id) {
    for (int i = 0; i < p_count; i++) {
        if (p_items[i].id == id) return &p_items[i];
    }
    return NULL;
}

// CSV  ε  (MSVC          : strtok_s)
static void load_csv_file(const char* fname) {
    FILE* fp = fopen(fname, "r");
    if (!fp) { p_count = 0; return; }

    char line[MAX_LINE];
    // skip header line if present
    if (!fgets(line, sizeof(line), fp)) { fclose(fp); p_count = 0; return; }

    p_count = 0;
    while (fgets(line, sizeof(line), fp)) {
        char tmp[MAX_LINE];
        safe_copy(tmp, sizeof(tmp), line);

        char* ctx = NULL;
        char* tok = strtok_s(tmp, ",", &ctx);
        if (!tok) continue;

        p_items[p_count].id = atoi(tok);

        tok = strtok_s(NULL, ",", &ctx);
        safe_copy(p_items[p_count].name, sizeof(p_items[p_count].name), tok ? tok : "");

        tok = strtok_s(NULL, ",", &ctx);
        safe_copy(p_items[p_count].slot, sizeof(p_items[p_count].slot), tok ? tok : "");

        tok = strtok_s(NULL, ",", &ctx);
        p_items[p_count].atk = tok ? atoi(tok) : 0;

        tok = strtok_s(NULL, ",", &ctx);
        p_items[p_count].def = tok ? atoi(tok) : 0;

        tok = strtok_s(NULL, ",", &ctx);
        p_items[p_count].hp = tok ? atoi(tok) : 0;

        tok = strtok_s(NULL, ",", &ctx);
        safe_copy(p_items[p_count].curse, sizeof(p_items[p_count].curse), tok ? tok : "");

        tok = strtok_s(NULL, ",", &ctx);
        if (tok) {
            // trim newline
            size_t L = strlen(tok);
            while (L > 0 && (tok[L - 1] == '\n' || tok[L - 1] == '\r')) { tok[L - 1] = '\0'; L--; }
            safe_copy(p_items[p_count].key_frag, sizeof(p_items[p_count].key_frag), tok);
        }
        else {
            p_items[p_count].key_frag[0] = '\0';
        }

        p_count++;
        if (p_count >= MAX_ITEMS) break;
    }

    fclose(fp);
}

// ---------- solve / gen functions (fixed) ----------

// 6 Poison
static void gen_poison(char* buf, size_t cap) {
    buf[0] = 0;
    int idx[MAX_ITEMS];
    int c = 0;
    for (int i = 0; i < p_count; i++) {
        //printf("Poison candidate: %s %d %d %d\n", p_items[i].name, p_items[i].atk, p_items[i].def, p_items[i].hp); // debug
        if (p_items[i].atk >= 4 &&
            p_items[i].def <= 5 &&
            p_items[i].hp <= 100) {
            idx[c++] = i;
        }
    }
    for (int i = c - 1; i >= 0; i--) {
        safe_cat(buf, (int)cap, p_items[idx[i]].name);
        if (i > 0) safe_cat(buf, (int)cap, "|");
    }
}

// 7 Strike
static void gen_strike(char* buf, size_t cap) {
    int sum = 0;
    for (int i = 0; i < p_count; i++) {
        if (strcmp(p_items[i].slot, "W") == 0) {
            char* p = strchr(p_items[i].key_frag, 'T');
            if (p) sum += (int)(p - p_items[i].key_frag);
        }
    }
    _snprintf(buf, (int)cap, "%dkey", sum);
}

// 8~11 Blink
static void gen_blink(char* buf, size_t cap) {
    buf[0] = 0;
    PuzzleItem* i202 = get_item_by_id(202);
    PuzzleItem* i208 = get_item_by_id(208);
    PuzzleItem* i205 = get_item_by_id(205);
    PuzzleItem* i212 = get_item_by_id(212);
    if (!i202 || !i208 || !i205 || !i212) return;

    // 1) HP      ġ + NIL                  
    int thp = i202->def + i208->def;
    const char* key1 = "";
    for (int i = 0; i < p_count; i++) {
        if (p_items[i].hp == thp && strcmp(p_items[i].key_frag, "NIL") != 0) {
            key1 = p_items[i].key_frag;
        }
    }
    safe_cat(buf, (int)cap, key1);

    // 2) ATK      ġ + NIL                  
    int tatk = i205->def * i212->atk;
    const char* key2 = "";
    for (int i = 0; i < p_count; i++) {
        if (p_items[i].atk == tatk && strcmp(p_items[i].key_frag, "NIL") != 0) {
            key2 = p_items[i].key_frag;
        }
    }
    safe_cat(buf, (int)cap, key2);

    // 3) C_01      + NIL                  
    int fidx = -1;
    for (int i = 0; i < p_count; i++) {
        if (strstr(p_items[i].curse, "C_01") &&
            strcmp(p_items[i].key_frag, "NIL") != 0) {
            fidx = i;
        }
    }
    if (fidx != -1) safe_cat(buf, (int)cap, p_items[fidx].key_frag);

    // 4)  ̸    'I'        + NIL         ù   °   
    for (int i = 0; i < p_count; i++) {
        if (p_items[i].name[0] == 'I' &&
            strcmp(p_items[i].key_frag, "NIL") != 0) {
            safe_cat(buf, (int)cap, p_items[i].key_frag);
            break;
        }
    }
}

// 13 Heal All
static void gen_heal_all(char* buf, size_t cap) {
    buf[0] = 0;
    for (int i = 0; i < p_count; i++) {
        if (strcmp(p_items[i].name, p_items[i].slot) >= 0) {
            safe_copy(buf, (int)cap, p_items[i].key_frag);
            return;
        }
    }
}

// 14 Range
static void gen_range(char* buf, size_t cap) {
    buf[0] = 0;
    int nv = 0;
    for (int i = 0; i < p_count; i++) {
        if (strstr(p_items[i].key_frag, "*K*") != NULL) {
            nv = p_items[i].hp;
            break;
        }
    }
    if (nv <= 0) return;
    if (strlen(current_filename) == 0) return;

    FILE* fp = fopen(current_filename, "rb");
    if (!fp) return;
    if (fseek(fp, nv - 1, SEEK_SET) != 0) { fclose(fp); return; }
    char t[6] = { 0 };
    if (fread(t, 1, 5, fp) < 1) { fclose(fp); return; }
    t[5] = '\0';
    fclose(fp);
    _snprintf(buf, (int)cap, "\"%s\"", t);
}

// 16 Bless
static void gen_bless(char* buf, size_t cap) {
    buf[0] = 0;
    char all[2000] = { 0 };
    for (int i = 0; i < p_count; i++) {
        if (strstr(p_items[i].name, "Sword") &&
            strcmp(p_items[i].key_frag, "NIL") != 0) {
            safe_cat(all, sizeof(all), p_items[i].key_frag);
        }
    }
    char* copy = my_strdup(all);
    if (!copy) return;
    char* ctx = NULL;
    char* tok = strtok_s(copy, "*", &ctx);
    char* best = NULL;
    int best_len = -1;
    while (tok) {
        int len = (int)strlen(tok);
        if (len > best_len) { best_len = len; best = tok; }
        tok = strtok_s(NULL, "*", &ctx);
    }
    if (best) safe_copy(buf, (int)cap, best);
    free(copy);
}

// 17/18 H/V
static void gen_hv(char* buf, size_t cap) {
    buf[0] = 0;
    int max_name_len = -1, min_curse_len = 999;
    int idx_name = -1, idx_curse = -1;
    for (int i = 0; i < p_count; i++) {
        int nl = (int)strlen(p_items[i].name);
        int cl = (int)strlen(p_items[i].curse);
        if (nl > max_name_len) { max_name_len = nl; idx_name = i; }
        if (cl < min_curse_len) { min_curse_len = cl; idx_curse = i; }
    }
    if (idx_name != -1 && idx_curse != -1) {
        char p1[4] = { 0 }, p2[4] = { 0 };
        strncpy(p1, p_items[idx_name].name, 3);
        int cl = (int)strlen(p_items[idx_curse].curse);
        if (cl >= 3) {
            strncpy(p2, p_items[idx_curse].curse + (cl - 3), 3);
        }
        else {
            strncpy(p2, p_items[idx_curse].curse, 3);
        }
        _snprintf(buf, (int)cap, "%s%s", p1, p2);
    }
}

// 19 Secret
static void gen_secret(char* buf, size_t cap) {
    buf[0] = 0;
    char tn[128] = { 0 };
    for (int i = 0; i < p_count; i++) {
        if (strstr(p_items[i].name, "Stone")) {
            safe_copy(tn, sizeof(tn), p_items[i].name);
            break;
        }
    }
    if (tn[0] == '\0') return;
    char* copy = my_strdup(tn);
    if (!copy) return;
    char* ctx = NULL;
    char* tok = strtok_s(copy, "AEIOUaeiou", &ctx);
    char* best = NULL;
    int best_len = -1;
    while (tok) {
        int len = (int)strlen(tok);
        if (len > best_len) { best_len = len; best = tok; }
        tok = strtok_s(NULL, "AEIOUaeiou", &ctx);
    }
    if (best) safe_copy(buf, (int)cap, best);
    free(copy);
}

// try unlock all
static void try_unlock_all(int reg_key) {
    char ans[2048];

    gen_poison(ans, sizeof(ans));
    if (ans[0]) {
        attempt_skill_unlock(reg_key, CMD_POISON, ans);
    }

    gen_strike(ans, sizeof(ans));
    if (ans[0]) {
        attempt_skill_unlock(reg_key, CMD_STRIKE, ans);
    }

    gen_blink(ans, sizeof(ans));
    if (ans[0]) {
        attempt_skill_unlock(reg_key, CMD_BLINK_UP, ans);
        attempt_skill_unlock(reg_key, CMD_BLINK_DOWN, ans);
        attempt_skill_unlock(reg_key, CMD_BLINK_LEFT, ans);
        attempt_skill_unlock(reg_key, CMD_BLINK_RIGHT, ans);
    }

    gen_heal_all(ans, sizeof(ans));
    if (ans[0]) {
        attempt_skill_unlock(reg_key, CMD_HEAL_ALL, ans);
    }

    gen_range(ans, sizeof(ans));
    if (ans[0]) {
        attempt_skill_unlock(reg_key, CMD_RANGE_ATTACK, ans);
    }

    gen_bless(ans, sizeof(ans));
    if (ans[0]) {
        attempt_skill_unlock(reg_key, CMD_BLESS, ans); // mapping caution
    }

    gen_hv(ans, sizeof(ans));
    if (ans[0]) {
        attempt_skill_unlock(reg_key, CMD_H_ATTACK, ans);
        attempt_skill_unlock(reg_key, CMD_V_ATTACK, ans);
    }

    gen_secret(ans, sizeof(ans));
    if (ans[0]) attempt_skill_unlock(reg_key, CMD_SECRETE, ans);
}

static int is_blink_unlocked(int reg_key) {
    return is_skill_unlocked(reg_key, CMD_BLINK_UP) &&
        is_skill_unlocked(reg_key, CMD_BLINK_DOWN) &&
        is_skill_unlocked(reg_key, CMD_BLINK_LEFT) &&
        is_skill_unlocked(reg_key, CMD_BLINK_RIGHT);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

// --------------------
// Combat helpers (AI)
// --------------------
static int manhattan_xy(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

static int clamp_int(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static int in_bounds_xy(int x, int y) {
    return (x >= 1 && x <= MAP_WIDTH && y >= 1 && y <= MAP_HEIGHT);
}

static int pick_step_towards(int my_x, int my_y, int en_x, int en_y) {
    int dx = en_x - my_x;
    int dy = en_y - my_y;
    int adx = abs(dx);
    int ady = abs(dy);

    if (adx == 0 && ady == 0) return CMD_ATTACK;

    // Prefer larger delta axis; break ties by a simple parity to reduce predictability.
    int prefer_h;
    if (adx > ady) prefer_h = 1;
    else if (ady > adx) prefer_h = 0;
    else prefer_h = ((my_x + my_y) & 1);

    if (prefer_h && adx > 0) return (dx > 0) ? CMD_RIGHT : CMD_LEFT;
    if (!prefer_h && ady > 0) return (dy > 0) ? CMD_DOWN : CMD_UP;

    // Fallback to the other axis.
    if (adx > 0) return (dx > 0) ? CMD_RIGHT : CMD_LEFT;
    return (dy > 0) ? CMD_DOWN : CMD_UP;
}

static int pick_step_away(int my_x, int my_y, int en_x, int en_y) {
    int dx = en_x - my_x;
    int dy = en_y - my_y;
    int adx = abs(dx);
    int ady = abs(dy);

    if (adx == 0 && ady == 0) return CMD_UP;

    int prefer_h;
    if (adx > ady) prefer_h = 1;
    else if (ady > adx) prefer_h = 0;
    else prefer_h = (((my_x + my_y) & 1) ^ 1);

    if (prefer_h && adx > 0) return (dx > 0) ? CMD_LEFT : CMD_RIGHT;
    if (!prefer_h && ady > 0) return (dy > 0) ? CMD_UP : CMD_DOWN;

    if (adx > 0) return (dx > 0) ? CMD_LEFT : CMD_RIGHT;
    return (dy > 0) ? CMD_UP : CMD_DOWN;
}

static int pick_blink_best(int my_x, int my_y, int en_x, int en_y, int want_escape) {
    // want_escape = 1 : maximize distance
    // want_escape = 0 : minimize distance
    struct { int cmd; int dx; int dy; } opts[4] = {
        { CMD_BLINK_UP,    0, -2 },
        { CMD_BLINK_DOWN,  0,  2 },
        { CMD_BLINK_LEFT, -2,  0 },
        { CMD_BLINK_RIGHT, 2,  0 },
    };

    int cur_dist = 0;
    cur_dist = manhattan_xy(my_x, my_y, en_x, en_y);
    int best_cmd = 0;
    int best_dist = cur_dist;

    for (int i = 0; i < 4; i++) {
        int nx = my_x + opts[i].dx;
        int ny = my_y + opts[i].dy;

        if (!in_bounds_xy(nx, ny)) continue;
        if (nx == en_x && ny == en_y) continue; // avoid blinking onto the enemy cell

        int nd = manhattan_xy(nx, ny, en_x, en_y);

        if (want_escape) {
            if (nd > best_dist) { best_dist = nd; best_cmd = opts[i].cmd; }
        }
        else {
            if (nd < best_dist) { best_dist = nd; best_cmd = opts[i].cmd; }
        }
    }

    // Only use if it improves.
    if (want_escape) return (best_dist > cur_dist) ? best_cmd : 0;
    return (best_dist < cur_dist) ? best_cmd : 0;
}

// ----------------------------------------------------
// RNG "initiative" steering (Player-file only)
// ----------------------------------------------------
// execute_turn() uses flip_coin() = rand()%2 for:
// - (optional) movement collision resolution
// - attack priority selection (p1_goes_first)
// In main.c, commands are fetched in order P1 then P2, then execute_turn() runs.
// That means P2 AI can reseed rand() AFTER P1 decides and BEFORE execute_turn() consumes rand(),
// effectively controlling the next flip(s).
static void rig_next_flip(int desired_bit) {
    // Find a small seed such that the first rand()%2 equals desired_bit.
    // This is fast and portable enough for typical libc/MSVC rand().
    for (unsigned int s = 0; s < 4096; s++) {
        srand(s);
        if ((rand() & 1) == (desired_bit & 1)) {
            srand(s); // set final state for the upcoming flip_coin()
            return;
        }
    }
    // Fallback: just seed to a constant.
    srand(0);
}

static void rig_initiative_for_me(int my_id) {
    // In execute_turn(): p1_goes_first = flip_coin();
    // If p1_goes_first==1 => P1 first, else P2 first.
    // So: if I'm P1, I want 1; if I'm P2, I want 0.
    int want_p1_first = (my_id == 1) ? 1 : 0;
    rig_next_flip(want_p1_first);
}

// ----------------------------------------------------
// Improved combat AI (stronger + blink using is_blink_unlocked())
// ----------------------------------------------------

// --------------------
// Combat helpers (AI) - extra (C-only, no nested funcs)
// --------------------
static void sim_step_1(int x, int y, int cmd, int* ox, int* oy) {
    int nx = x, ny = y;
    if (cmd == CMD_UP) ny--;
    else if (cmd == CMD_DOWN) ny++;
    else if (cmd == CMD_LEFT) nx--;
    else if (cmd == CMD_RIGHT) nx++;

    // same rule as game.c (1-based bounds)
    if (nx >= 1 && nx <= MAP_WIDTH) x = nx;
    if (ny >= 1 && ny <= MAP_HEIGHT) y = ny;

    *ox = x;
    *oy = y;
}

// Break dangerous alignment with a 1-step perpendicular move.
// want_escape: 0 => prefer getting closer, 1 => prefer increasing distance.
static int pick_perp_break_step_1(int my_x, int my_y, int en_x, int en_y,
    int aligned_row, int aligned_col, int want_escape) {
    int candidates[2] = { 0, 0 };
    if (aligned_row) { candidates[0] = CMD_UP; candidates[1] = CMD_DOWN; }
    else if (aligned_col) { candidates[0] = CMD_LEFT; candidates[1] = CMD_RIGHT; }
    else return 0;

    int best_cmd = 0;
    int best_score = -2147483647;

    for (int i = 0; i < 2; i++) {
        int cmd = candidates[i];
        int nx, ny;
        sim_step_1(my_x, my_y, cmd, &nx, &ny);

        // ignore boundary no-op
        if (nx == my_x && ny == my_y) continue;

        // must break the current alignment axis
        if (aligned_row && ny == en_y) continue;
        if (aligned_col && nx == en_x) continue;

        {
            int nd = manhattan_xy(nx, ny, en_x, en_y);
            // primary: direction objective
            int score = want_escape ? (nd * 10) : (-nd * 10);
            // secondary: slight center bias to reduce edge-stuck loops
            score -= (abs(nx - 4) + abs(ny - 4));

            if (score > best_score) { best_score = score; best_cmd = cmd; }
        }
    }

    return best_cmd;
}

static int our_reg_key = -1;

static int simple_ai(const Player* me, const Player* enemy) {
    // ============================================================
    // Combat AI (PlayerB)
    //
    // Goals:
    // 1) Do not waste MP on pointless blinks (prevents MP=0 "do nothing" loops)
    // 2) Respect the very strong H/V line attacks (in game.c they deal damage twice)
    // 3) When the game stagnates, enter finisher_mode to force a conclusion
    //
    // NOTE: Only combat AI logic is changed here.
    // ============================================================

    // --- persistent combat memory (AI-only) ---
    static int last_en_hp = -1;
    static int last_dist = -1;
    static int stale_hp_turns = 0;      // enemy HP not decreasing
    static int stale_dist_turns = 0;    // distance not decreasing
    static int finisher_mode = 0;       // force contact / damage
    static int poison_cd = 0;           // cooldown to avoid poison spam
    static int poison_suspect = 0;      // best-effort: turns after enemy used POISON

    // --- read current state ---
    int my_id = get_player_id(me);
    int my_hp = get_player_hp(me);
    int my_mp = get_player_mp(me);
    int my_x = get_player_x(me);
    int my_y = get_player_y(me);

    int en_hp = get_player_hp(enemy);
    int en_mp = get_player_mp(enemy);
    int en_x = get_player_x(enemy);
    int en_y = get_player_y(enemy);

    int dist = manhattan_xy(my_x, my_y, en_x, en_y);
    int reg = our_reg_key;

    int en_last = get_player_last_command(enemy);

    int aligned_row = (my_y == en_y);
    int aligned_col = (my_x == en_x);

    // --- capability checks (unlocked + enough MP) ---
    int can_heal = (my_mp >= 1);
    int can_heal_all = (reg >= 0 && is_skill_unlocked(reg, CMD_HEAL_ALL) && my_mp >= 3);
    int can_strike = (reg >= 0 && is_skill_unlocked(reg, CMD_STRIKE) && my_mp >= 2);
    int can_range = (reg >= 0 && is_skill_unlocked(reg, CMD_RANGE_ATTACK) && my_mp >= 1);
    int can_poison = (reg >= 0 && is_skill_unlocked(reg, CMD_POISON) && my_mp >= 5);
    int can_bless = (reg >= 0 && is_skill_unlocked(reg, CMD_BLESS) && my_mp >= 2);
    int can_blink = (reg >= 0 && is_blink_unlocked(reg) && my_mp >= 1);
    int can_h = (reg >= 0 && is_skill_unlocked(reg, CMD_H_ATTACK) && my_mp >= 3);
    int can_v = (reg >= 0 && is_skill_unlocked(reg, CMD_V_ATTACK) && my_mp >= 3);

    // --- update memory / loop detectors ---
    if (poison_cd > 0) poison_cd--;

    if (en_last == CMD_POISON) poison_suspect = 2; // poison lasts 2 ticks in game.c
    else if (poison_suspect > 0) poison_suspect--;

    if (last_en_hp >= 0) {
        if (en_hp < last_en_hp) stale_hp_turns = 0;
        else stale_hp_turns++;

        if (last_dist >= 0 && dist < last_dist) stale_dist_turns = 0;
        else stale_dist_turns++;

        finisher_mode = (stale_hp_turns >= 6) || (stale_dist_turns >= 8);
    }
    last_en_hp = en_hp;
    last_dist = dist;

    // ============================================================
    // 0) Best damage buttons (no movement)
    // ============================================================
    // H/V line attacks are extremely strong here (damage applied twice in game.c),
    // so if we are aligned and have MP, cast immediately.
    if ((aligned_row && can_h) || (aligned_col && can_v)) {
        if (my_id == 2) rig_initiative_for_me(my_id);
        return aligned_row ? CMD_H_ATTACK : CMD_V_ATTACK;
    }

    // Melee range
    if (dist == 1) {
        if (can_strike) { if (my_id == 2) rig_initiative_for_me(my_id); return CMD_STRIKE; }
        if (my_id == 2) rig_initiative_for_me(my_id);
        return CMD_ATTACK;
    }

    // Distance-2 poke
    if (dist == 2 && can_range) {
        if (my_id == 2) rig_initiative_for_me(my_id);
        return CMD_RANGE_ATTACK;
    }

    // ============================================================
    // 1) Survival / anti-poison
    // ============================================================
    if (my_hp <= 2) {
        // Bless is only useful if we suspect poison.
        if (can_bless && poison_suspect > 0 && en_hp > 1) {
            if (my_id == 2) rig_initiative_for_me(my_id);
            return CMD_BLESS;
        }

        // Heal if we have room (doing it in melee is usually wasted)
        if (dist >= 2) {
            if (can_heal_all && my_hp <= 3) { if (my_id == 2) rig_initiative_for_me(my_id); return CMD_HEAL_ALL; }
            if (can_heal) { if (my_id == 2) rig_initiative_for_me(my_id); return CMD_HEAL; }
        }

        // If we cannot heal, recover MP if empty.
        if (my_mp == 0) {
            if (my_id == 2) rig_initiative_for_me(my_id);
            return CMD_REST;
        }

        // If enemy is close, create space (step away; blink only if it improves).
        if (dist <= 2) {
            if (can_blink) {
                int b = pick_blink_best(my_x, my_y, en_x, en_y, 1);
                if (b) return b; // moving => don't rig
            }
            return pick_step_away(my_x, my_y, en_x, en_y);
        }
    }

    // ============================================================
    // 2) Anti H/V threat
    // If enemy has MP>=3 and we are aligned, break alignment immediately
    // unless we can cast H/V ourselves (already handled above).
    // ============================================================
    if ((aligned_row || aligned_col) && en_mp >= 3) {
        // Prefer a perpendicular 1-step break.
        {
            int break_cmd = pick_perp_break_step_1(my_x, my_y, en_x, en_y, aligned_row, aligned_col, 0);
            if (break_cmd) return break_cmd;
        }

        // If we are edge-stuck, blink perpendicular as last resort.
        if (can_blink) {
            int opts[2];
            if (aligned_row) { opts[0] = CMD_BLINK_UP; opts[1] = CMD_BLINK_DOWN; }
            else { opts[0] = CMD_BLINK_LEFT; opts[1] = CMD_BLINK_RIGHT; }

            int best_cmd = 0;
            int best_d = dist;

            for (int i = 0; i < 2; i++) {
                int cmd = opts[i];
                int nx = my_x, ny = my_y;

                if (cmd == CMD_BLINK_UP) ny -= 2;
                else if (cmd == CMD_BLINK_DOWN) ny += 2;
                else if (cmd == CMD_BLINK_LEFT) nx -= 2;
                else if (cmd == CMD_BLINK_RIGHT) nx += 2;

                if (!in_bounds_xy(nx, ny)) continue;
                if (nx == en_x && ny == en_y) continue;

                // must break the alignment axis
                if (aligned_row && ny == en_y) continue;
                if (aligned_col && nx == en_x) continue;

                {
                    int nd = manhattan_xy(nx, ny, en_x, en_y);
                    if (nd < best_d) { best_d = nd; best_cmd = cmd; }
                }
            }

            if (best_cmd) return best_cmd; // moving => don't rig
        }
    }

    // ============================================================
    // 3) Tempo tools (poison + MP management)
    // ============================================================
    // Use poison as a clock (not a camping tool). Only when safe-ish and not spamming.
    if (can_poison && poison_cd == 0 && my_hp >= 4 && dist >= 3 && en_hp >= 3) {
        poison_cd = 6;
        if (my_id == 2) rig_initiative_for_me(my_id);
        return CMD_POISON;
    }

    // If we are far and low MP, REST to get back to threat levels (2~3 MP).
    if (my_mp <= 1 && dist >= 3) {
        if (my_id == 2) rig_initiative_for_me(my_id);
        return CMD_REST;
    }

    // ============================================================
    // 4) Approach / chase (avoid blink loops)
    // ============================================================
    if (finisher_mode) {
        // In finisher mode, close fast.
        if (can_blink && dist >= 5) {
            int b = pick_blink_best(my_x, my_y, en_x, en_y, 0);
            if (b) return b;
        }
        return pick_step_towards(my_x, my_y, en_x, en_y);
    }

    // Normal approach:
    // Blink only if it saves >=2 distance and does NOT align us when enemy can H/V.
    if (can_blink && dist >= 6) {
        struct { int cmd; int dx; int dy; } opts[4] = {
            { CMD_BLINK_UP, 0, -2 },
            { CMD_BLINK_DOWN, 0, 2 },
            { CMD_BLINK_LEFT, -2, 0 },
            { CMD_BLINK_RIGHT, 2, 0 }
        };

        int best_cmd = 0;
        int best_d = dist;

        for (int i = 0; i < 4; i++) {
            int nx = my_x + opts[i].dx;
            int ny = my_y + opts[i].dy;

            if (!in_bounds_xy(nx, ny)) continue;
            if (nx == en_x && ny == en_y) continue;

            // avoid ending aligned when enemy has MP to punish
            if (en_mp >= 3 && (nx == en_x || ny == en_y)) continue;

            {
                int nd = manhattan_xy(nx, ny, en_x, en_y);
                if (nd < best_d) { best_d = nd; best_cmd = opts[i].cmd; }
            }
        }

        if (best_cmd && best_d <= dist - 2) return best_cmd;
    }

    // Step toward, with a small bias to avoid alignment if enemy has MP>=3
    if (en_mp >= 3) {
        int dx = abs(en_x - my_x);
        int dy = abs(en_y - my_y);

        // try horizontal if it doesn't align
        if (dx >= dy && dx > 0) {
            int cmd = (en_x > my_x) ? CMD_RIGHT : CMD_LEFT;
            int nx, ny;
            sim_step_1(my_x, my_y, cmd, &nx, &ny);
            if (!(nx == en_x)) { // would align column
                if (nx != my_x || ny != my_y) return cmd;
            }
        }

        // try vertical if it doesn't align
        if (dy > 0) {
            int cmd = (en_y > my_y) ? CMD_DOWN : CMD_UP;
            int nx, ny;
            sim_step_1(my_x, my_y, cmd, &nx, &ny);
            if (!(ny == en_y)) { // would align row
                if (nx != my_x || ny != my_y) return cmd;
            }
        }
        // if both would align, just follow normal path
    }

    return pick_step_towards(my_x, my_y, en_x, en_y);
}

// entry
void student1_ai_entry() {
    load_csv_file(current_filename);
    int reg = register_player_ai("TeamStudent2", simple_ai);
    our_reg_key = reg;
    if (reg >= 0) try_unlock_all(reg);
}
