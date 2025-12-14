/*
 * =================================================================================================
    조장 : 정진욱
    코드 하나로 합친 사람 : 정진욱
    AI 제작 : 정진욱
    문제 푼 사람
    1, 8번 문제 : 정진욱
    2, 7번 문제 : 김용환
    3, 4번 문제 : 김한슬
    5, 6번 문제 : 정형욱
 * =================================================================================================
 */

#define _CRT_SECURE_NO_WARNINGS
#include "api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAP_WIDTH 7
#define MAP_HEIGHT 7

int g_reg_key;

/* -------------------------------------------------------
   Item 구조체 정의 (CSV 한 줄 = 하나의 아이템 데이터)
------------------------------------------------------- */
typedef struct {
    int id;
    char name[64];
    char slot[8];
    int atk;
    int def;
    int hp;
    char curse[32];
    char key_frag[32];
} Item;

/* -------------------------------------------------------
   CSV 파일 로딩 함수 (AI1-2_C_Final.csv 읽기)
   - CSV 내용을 Item 구조체 배열에 저장
   - 읽은 아이템 개수를 반환
------------------------------------------------------- */

int load_csv(Item items[], int max_items) {
    FILE* fp = fopen("game_puzzle_en.csv", "r");
    if (!fp) {
        printf("[ERROR] CSV 파일을 찾을 수 없습니다: AI1-2_C_Final.csv\n");
        return 0;
    }

    char line[512];
    int count = 0;

    /* 헤더(첫 줄) 제거 */
    if (!fgets(line, sizeof(line), fp)) { fclose(fp); return 0; }

    /* CSV 내용 한 줄씩 읽기 */
    while (fgets(line, sizeof(line), fp) && count < max_items) {
        Item* it = &items[count];

        /* CSV 형식: ID,NAME,SLOT,ATK,DEF,HP,CURSE,KEY_FRAG */
        int ret = sscanf(line, "%d,%63[^,],%7[^,],%d,%d,%d,%31[^,],%31s",
            &it->id,
            it->name,
            it->slot,
            &it->atk,
            &it->def,
            &it->hp,
            it->curse,
            it->key_frag
        );

        /* 실패 시에도 index는 그대로 유지 */
        if (ret < 8) {
            /* 파싱 실패 시 단순 통과 */
        }
        count++;
    }

    fclose(fp);
    return count;
}

/* -------------------------------------------------------
   문제 1 (독해금 6번 스킬)
   - ATK >= 4
   - DEF <= 5
   - HP <= 100
   조건을 모두 만족하는 아이템들의 NAME을 모은 뒤
   파일 등장 "역순" 으로 정렬하여
   "A|B|C" 형태의 문자열로 반환
------------------------------------------------------- */
void solve_skill_6(char* out, int outlen) {
    out[0] = '\0';

    Item items[256];
    int n = load_csv(items, 256);

    /* 조건을 만족하는 이름을 임시 저장 */
    const char* selected[256];
    int count = 0;

    for (int i = 0; i < n; i++) {
        if (items[i].atk >= 4 &&
            items[i].def <= 5 &&
            items[i].hp <= 100)
        {
            selected[count++] = items[i].name;
        }
    }

    /* 역순으로 붙이기 */
    for (int i = count - 1; i >= 0; i--) {
        strncat(out, selected[i], outlen - strlen(out) - 1);
        if (i != 0) {
            strncat(out, "|", outlen - strlen(out) - 1);
        }
    }

    //printf("[DEBUG] 문제1(스킬6) 정답 = %s\n", out);
}

/* -------------------------------------------------------
   문제 2 - s7 전역 변수에 정답 저장
------------------------------------------------------- */
char s7[64] = { 0 };
void solve_skill_7(void)
{
    FILE* fp = fopen("game_puzzle_en.csv", "r");
    char line[512];

    int total = 0;

    fgets(line, sizeof(line), fp); // 헤더 한 줄 버림
    while (fgets(line, sizeof(line), fp)) // csv 토큰화
    {
        char* id = strtok(line, ",");
        char* name = strtok(NULL, ",");
        char* slot = strtok(NULL, ",");
        char* atk = strtok(NULL, ",");
        char* def = strtok(NULL, ",");
        char* hp = strtok(NULL, ",");
        char* curse = strtok(NULL, ",");
        char* key_frag = strtok(NULL, ",");

        if (!slot || !key_frag) continue; // 주요 문자열 정상 여부 검사

        if (slot[0] == 'W') // W인 무기 검출
        {
            int index = 0;
            char* pos = strchr(key_frag, 'T');
            if (pos)
            {
                index = (int)(pos - key_frag); // T의 위치 검출
            }
            total += index; // 합산
        }
    }

    fclose(fp);

    char answer2[32];
    sprintf(answer2, "%d", total); // int인 total을 string으로 출력해주는 sprintf 사용
    strcat(answer2, "key"); // "key" 붙이기
    strcpy(s7, answer2);
}

/* =========================================
   문제 3 : 점멸 해금 (CMD 8~11)
   strcat & 조건 조합 문제 (교수님 수정 조건 반영)
   ========================================= */
void solve_skill_8_11(char* out) {
    out[0] = '\0';

    Item items[128];
    int n = load_csv(items, 128);

    char f1[32] = "", f2[32] = "", f3[32] = "", f4[32] = "";

    /* 조건 1
       ID 202 DEF + ID 208 DEF = 어떤 아이템의 HP */
    int def202 = 0, def208 = 0;
    for (int i = 0; i < n; i++) {
        if (items[i].id == 202) def202 = items[i].def;
        if (items[i].id == 208) def208 = items[i].def;
    }
    int target_hp = def202 + def208;

    for (int i = 0; i < n; i++) {
        if (items[i].hp == target_hp &&
            strcmp(items[i].key_frag, "NIL") != 0) {
            strcpy(f1, items[i].key_frag);
            break;
        }
    }

    /* 조건 2 (★ 교수님 수정 사항 ★)
       ID 205의 DEF * ID 212의 ATK = 어떤 아이템의 ATK */
    int def205 = 0, atk212 = 0;
    for (int i = 0; i < n; i++) {
        if (items[i].id == 205) def205 = items[i].def;
        if (items[i].id == 212) atk212 = items[i].atk;
    }
    int target_atk = def205 * atk212;

    for (int i = 0; i < n; i++) {
        if (items[i].atk == target_atk &&
            strcmp(items[i].key_frag, "NIL") != 0) {
            strcpy(f2, items[i].key_frag);
            break;
        }
    }

    /* 조건 3
       CURSE에 "C_01" 포함 + 파일에서 가장 마지막 */
    for (int i = 0; i < n; i++) {
        if (strstr(items[i].curse, "C_01") &&
            strcmp(items[i].key_frag, "NIL") != 0) {
            strcpy(f3, items[i].key_frag);
        }
    }

    /* 조건 4
       NAME이 'I'로 시작 + 파일에서 가장 처음 */
    for (int i = 0; i < n; i++) {
        if (items[i].name[0] == 'I' &&
            strcmp(items[i].key_frag, "NIL") != 0) {
            strcpy(f4, items[i].key_frag);
            break;
        }
    }

    /* strcat으로 순서대로 결합 */
    strcat(out, f1);
    strcat(out, f2);
    strcat(out, f3);
    strcat(out, f4);

    //printf("[문제 4 BLINK 결과] %s\n", out);
}


/* -------------------------------------------------------
   문제 4 (HEAL2 스킬)
   - strcmp(NAME, SLOT) 결과가
     0 또는 양수(사전순으로 NAME이 더 뒤일 때)
     첫 번째 아이템의 KEY_FRAG 반환
------------------------------------------------------- */
void solve_skill_13(char* out, int outlen) {
    Item items[128];
    int n = load_csv(items, 128);
    out[0] = '\0';

    for (int i = 0; i < n; ++i) {
        if (strcmp(items[i].key_frag, "NIL") == 0) continue;

        if (strcmp(items[i].name, items[i].slot) >= 0) {
            strncpy(out, items[i].key_frag, outlen - 1);
            out[outlen - 1] = '\0';
            //printf("[DEBUG] 문제13 선택: id=%d name=%s slot=%s frag=%s\n", items[i].id, items[i].name, items[i].slot, out);
            return;
        }
    }

    //printf("[DEBUG] 문제13: 해당 조건에 맞는 행이 없음\n");
}

/* -------------------------------------------------------
   문제 5 (원거리공격 / CMD_RANGE_ATTACK = 14)
   - KEY_FRAG가 'K' 또는 'k' 포함된 아이템 찾기
   - 그 아이템의 HP를 N으로 정의
   - 파일 시작에서 N 바이트 앞으로 fseek
   - 거기서 5바이트 읽고, "<5바이트>" 형태로 out에 저장

   오프셋은 0부터 시작합니다.
   문제에서 'N 바이트 앞으로 이동'이면 fseek(fp, N, SEEK_SET)가 맞습니다.
------------------------------------------------------- */
static void solve_skill_14(char* out, int outlen) {
    out[0] = '\0';

    Item items[256];
    int n = load_csv(items, 256);

    int hpN = -1;

    for (int i = 0; i < n; i++) {
        const char* kf = items[i].key_frag;
        if (strcmp(kf, "NIL") == 0) continue;

        /* 'K' 또는 'k' 포함 (교수님이 *k*로 바꿔도 대응) */
        if (strchr(kf, 'K') || strchr(kf, 'k')) {
            hpN = items[i].hp;
            break;
        }
    }

    if (hpN < 0) {
        printf("[ERROR] KEY_FRAG에 K/k 포함된 아이템을 못 찾음\n");
        return;
    }

    FILE* fp = fopen("game_puzzle_en.csv", "rb");
    if (!fp) {
        printf("[ERROR] game_puzzle_en.csv 바이너리 열기 실패\n");
        return;
    }

    if (fseek(fp, hpN - 1, SEEK_SET) != 0) {
        fclose(fp);
        printf("[ERROR] fseek 실패 (N=%d)\n", hpN);
        return;
    }

    char buf[6] = { 0 };
    (void)fread(buf, 1, 5, fp);
    fclose(fp);

    /* 큰따옴표 포함 문자열: "LOT,A" 같은 형태 */
    snprintf(out, outlen, "\"%s\"", buf);
}

/* -------------------------------------------------------
   문제 6 (스킬 16 / CMD_BLESS = 16)
   - NAME에 "Sword" 포함된 아이템들의 KEY_FRAG를 순서대로 이어붙여 S 생성
   - S를 '*' 기준으로 strtok
   - 토큰 중 가장 긴 것 선택 (길이 같으면 먼저 나온 토큰 유지)
   - out에 최장 토큰 저장
------------------------------------------------------- */
static void solve_skill_16(char* out, int outlen) {
    out[0] = '\0';

    Item items[256];
    int n = load_csv(items, 256);

    char S[512] = { 0 };
    int found = 0;

    for (int i = 0; i < n; i++) {
        if (strcmp(items[i].key_frag, "NIL") == 0) continue;

        if (strstr(items[i].name, "Sword") != NULL || strstr(items[i].name, "sword") != NULL) {
            strncat(S, items[i].key_frag, (int)sizeof(S) - (int)strlen(S) - 1);
            found++;
        }
    }

    if (found == 0) {
        printf("[ERROR] name에 Sword 포함된 아이템이 없음\n");
        return;
    }

    char copy[512];
    strncpy(copy, S, sizeof(copy) - 1);
    copy[sizeof(copy) - 1] = '\0';

    char best[128] = { 0 };

    for (char* tok = strtok(copy, "*"); tok != NULL; tok = strtok(NULL, "*")) {
        int len = (int)strlen(tok);
        if (len == 0) continue;
        if (len > (int)strlen(best)) {
            strncpy(best, tok, sizeof(best) - 1);
            best[sizeof(best) - 1] = '\0';
        }
    }

    strncpy(out, best, outlen - 1);
    out[outlen - 1] = '\0';
}

/* -------------------------------------------------------
   문제 7 - s7 전역 변수에 정답 저장
------------------------------------------------------- */
char s17[64] = { 0 };
void solve_skill_17_18(void)
{
    FILE* fp = fopen("game_puzzle_en.csv", "r");
    char line[512];

    char longest_name[128] = "";
    char shortest_curse[128] = "";
    int max_len = -1;
    int min_len = 9999;

    fgets(line, sizeof(line), fp); // 헤더 한 줄 버림
    while (fgets(line, sizeof(line), fp)) // csv 토큰화
    {
        char* id = strtok(line, ",");
        char* name = strtok(NULL, ",");
        char* slot = strtok(NULL, ",");
        char* atk = strtok(NULL, ",");
        char* def = strtok(NULL, ",");
        char* hp = strtok(NULL, ",");
        char* curse = strtok(NULL, ",");
        char* key_frag = strtok(NULL, ",");

        if (!name || !curse) continue; // 주요 문자열 정상 여부 검사

        int name_len = strlen(name); // 가장 긴 문자열 찾기
        if (name_len > max_len)
        {
            max_len = name_len;
            strcpy(longest_name, name);
        }

        int curse_len = strlen(curse); // 가장 짧은 문자열 찾기
        if (curse_len < min_len)
        {
            min_len = curse_len;
            strcpy(shortest_curse, curse);
        }
    }

    fclose(fp);

    char part1[4];
    char part2[4];
    char answer7[7];

    memcpy(part1, longest_name, 3); // 앞 3글자 추출: memcpy 사용
    part1[3] = '\0'; // NULL 붙이기

    int len2 = strlen(shortest_curse); // 뒤 3글자 추출: memcpy 사용
    memcpy(part2, shortest_curse + (len2 - 3), 3);
    part2[3] = '\0'; // NULL 붙이기

    strcpy(answer7, part1); // 문자열 연결
    strcat(answer7, part2); // 문자열 연결

    //printf("%s\n", answer7);
    strcpy(s17, answer7);
}


/* -------------------------------------------------------
   문제 8 (스킬 19)
   - NAME에 Stone 포함 아이템 탐색
   - NAME 문자열을 모음(A,E,I,O,U)을 '개별' 구분자로 직접 분리
   - 가장 긴 토큰 선택 (동률이면 먼저 나온 것)
   - 중간 과정 디버그 출력 포함
------------------------------------------------------- */
void solve_skill_19(char* out, int outlen) {
    out[0] = '\0';

    Item items[256];
    int n = load_csv(items, 256);

    char selected_name[128] = { 0 };

    /* Stone 포함 NAME 찾기 */
    for (int i = 0; i < n; i++) {
        if (strstr(items[i].name, "Stone") != NULL) {
            strcpy(selected_name, items[i].name);
            //printf("[DEBUG] Stone 포함 NAME 발견 (id=%d) → %s\n", items[i].id, selected_name);
            break;
        }
    }

    if (!selected_name[0]) {
        printf("[DEBUG] Stone 포함 아이템 없음\n");
        return;
    }

    /* 모음 구분자(개별 문자 처리) */
    char vowels[] = { 'A', 'E', 'I', 'O', 'U', 'a', 'e', 'i', 'o', 'u', '\0' };

    //printf("[DEBUG] 구분자(모음) 목록 = ");
    /*
    for (int i = 0; vowels[i]; i++)
        printf("%c ", vowels[i]);
    printf("\n");
    */

    /* 직접 토큰 만들기 */
    char best[128] = { 0 };
    char current[128] = { 0 };
    int cidx = 0;
    int token_index = 0;

    //printf("[DEBUG] 문자열 분석 시작 = \"%s\"\n", selected_name);

    for (int i = 0; selected_name[i] != '\0'; i++) {
        char ch = selected_name[i];

        /* 모음인지 검사 */
        if (strchr(vowels, ch) != NULL) {
            /* 토큰 종료 */
            if (cidx > 0) {
                current[cidx] = '\0';
                //printf("[DEBUG] 토큰 %d = \"%s\" (길이=%d)\n", token_index, current, (int)strlen(current));

                if (strlen(current) > strlen(best)) {
                    //printf("        → 최장 토큰 갱신: \"%s\" → \"%s\"\n", best[0] ? best : "(none)", current);
                    strcpy(best, current);
                }

                token_index++;
                cidx = 0;
            }
        }
        else {
            /* 문자 저장 */
            current[cidx++] = ch;
        }
    }

    /* 마지막 토큰 처리 */
    if (cidx > 0) {
        current[cidx] = '\0';
        //printf("[DEBUG] 토큰 %d = \"%s\" (길이=%d)\n", token_index, current, (int)strlen(current));

        if (strlen(current) > strlen(best)) {
            //printf("        → 최장 토큰 갱신: \"%s\" → \"%s\"\n", best[0] ? best : "(none)", current);
            strcpy(best, current);
        }
    }

    strncpy(out, best, outlen - 1);
    out[outlen - 1] = '\0';

    //printf("[DEBUG] 최종 선택된 토큰 = \"%s\"\n\n", out);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// ============================================================
// 기본 유틸
// ============================================================
static inline int in_bounds_xy(int x, int y) {
    return (x >= 1 && x <= MAP_WIDTH &&
        y >= 1 && y <= MAP_HEIGHT);
}

// ============================================================
// 기본 접근 / 후퇴
// ============================================================
static inline int pick_axis_step_fast(
    int mx, int my, int dx, int dy, int away)
{
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;

    int prefer_h =
        (adx > ady) ? 1 :
        (ady > adx) ? 0 :
        ((mx + my + away) & 1);

    if (prefer_h && adx)
        return ((dx > 0) ^ away) ? CMD_RIGHT : CMD_LEFT;
    if (!prefer_h && ady)
        return ((dy > 0) ^ away) ? CMD_DOWN : CMD_UP;

    if (adx)
        return ((dx > 0) ^ away) ? CMD_RIGHT : CMD_LEFT;
    return ((dy > 0) ^ away) ? CMD_DOWN : CMD_UP;
}

// ============================================================
// 대응 이동 (행/열 회피)
// ============================================================
static inline int safe_move_against_sniper(
    int mx, int my, int ex, int ey, int away)
{
    int dx = ex - mx;
    int dy = ey - my;

    int cmd = pick_axis_step_fast(mx, my, dx, dy, away);

    int nx = mx, ny = my;
    if (cmd == CMD_UP) ny--;
    else if (cmd == CMD_DOWN) ny++;
    else if (cmd == CMD_LEFT) nx--;
    else if (cmd == CMD_RIGHT) nx++;

    // 같은 행 또는 열이면 다른 축으로 회피
    if (nx == ex || ny == ey) {
        if (cmd == CMD_LEFT || cmd == CMD_RIGHT) {
            return (dy > 0) ? CMD_DOWN : CMD_UP;
        }
        else {
            return (dx > 0) ? CMD_RIGHT : CMD_LEFT;
        }
    }
    return cmd;
}

// ============================================================
// BLINK 선택
// ============================================================
static inline int pick_blink_fast(
    int mx, int my, int dx, int dy, int escape)
{
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;

    int nx = mx, ny = my;
    int cmd = 0;

    if (adx >= ady) {
        nx += ((dx > 0) ^ escape) ? 2 : -2;
        cmd = ((dx > 0) ^ escape) ? CMD_BLINK_RIGHT : CMD_BLINK_LEFT;
    }
    else {
        ny += ((dy > 0) ^ escape) ? 2 : -2;
        cmd = ((dy > 0) ^ escape) ? CMD_BLINK_DOWN : CMD_BLINK_UP;
    }

    if (!in_bounds_xy(nx, ny)) return 0;
    return cmd;
}

// ============================================================
// BLINK 안전 필터
// ============================================================
static inline int safe_blink_against_sniper(
    int mx, int my, int ex, int ey, int escape)
{
    int dx = ex - mx;
    int dy = ey - my;

    int cmd = pick_blink_fast(mx, my, dx, dy, escape);
    if (!cmd) return 0;

    int nx = mx, ny = my;
    if (cmd == CMD_BLINK_LEFT)  nx -= 2;
    if (cmd == CMD_BLINK_RIGHT) nx += 2;
    if (cmd == CMD_BLINK_UP)    ny -= 2;
    if (cmd == CMD_BLINK_DOWN)  ny += 2;

    if (nx == ex || ny == ey)
        return 0;

    return cmd;
}

// ============================================================
// RNG 선공 조작
// ============================================================
static unsigned short seed_for_bit[2] = { 0,1 };
static int seed_init = 0;

static void init_seed_table(void) {
    if (seed_init) return;
    for (unsigned s = 0; s < 4096; s++) {
        srand(s);
        seed_for_bit[rand() & 1] = s;
    }
    seed_init = 1;
}

static inline void rig_initiative_for_me(int my_id) {
    init_seed_table();
    srand(seed_for_bit[(my_id == 1) ? 1 : 0]);
}

// ============================================================
// AI LOGIC
// ============================================================
static int student2_ai(const Player* me, const Player* enemy) {
    // ---- 메모리 ----
    static unsigned char poison_cd = 0;
    static unsigned char poison_active = 0;
    static int last_en_hp = 0;

    static int last_ex = -1, last_ey = -1;
    static unsigned char enemy_poison_stay = 0;

    // ---- 상태 ----
    int my_id = get_player_id(me);
    int my_hp = get_player_hp(me);
    int my_mp = get_player_mp(me);
    int mx = get_player_x(me);
    int my = get_player_y(me);

    int ex = get_player_x(enemy);
    int ey = get_player_y(enemy);
    int en_hp = get_player_hp(enemy);

    int dx = ex - mx;
    int dy = ey - my;
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;
    int dist = adx + ady;

    int cmd = 0;
    int need_rig = 0;

    // ---- 상태 갱신 ----
    if (poison_cd) poison_cd--;

    if (last_en_hp && en_hp < last_en_hp)
        poison_active = 2;
    else if (poison_active)
        poison_active--;

    last_en_hp = en_hp;

    // ---- 독 캠핑 탐지 ----
    if (get_player_last_command(enemy) == CMD_POISON &&
        last_ex == ex && last_ey == ey)
    {
        if (enemy_poison_stay < 3)
            enemy_poison_stay++;
    }
    else {
        enemy_poison_stay = 0;
    }

    last_ex = ex;
    last_ey = ey;

    // ========================================================
    // 독 캠핑 최우선 파훼
    // ========================================================
    if (enemy_poison_stay >= 2) {
        if (dist == 1) {
            need_rig = 1;
            cmd = (my_mp >= 2) ? CMD_STRIKE : CMD_ATTACK;
            goto END;
        }

        if (my_mp >= 1) {
            cmd = safe_blink_against_sniper(mx, my, ex, ey, 0);
            if (cmd) goto END;
        }

        cmd = safe_move_against_sniper(mx, my, ex, ey, 0);
        goto END;
    }

    // ========================================================
    // 근접 마무리
    // ========================================================
    if (dist == 1) {
        set_custom_secrete_message(g_reg_key, "펀치펀치!!");
        need_rig = 1;
        cmd = (my_mp >= 2) ? CMD_STRIKE : CMD_ATTACK;
        goto END;
    }

    // ========================================================
    // 독 사용
    // ========================================================
    if (my_mp >= 5 && poison_cd == 0 && en_hp >= 3) {
        poison_cd = 4;
        need_rig = 1;
        cmd = CMD_POISON;
        set_custom_secrete_message(g_reg_key, "독 공격!");
        goto END;
    }

    // ========================================================
    // 독 유지 거리
    // ========================================================
    if (poison_active) {
        if (dist <= 1) {
            if (my_mp >= 1) {
                cmd = safe_blink_against_sniper(mx, my, ex, ey, 1);
                if (cmd) goto END;
            }
            cmd = safe_move_against_sniper(mx, my, ex, ey, 1);
            goto END;
        }

        if (dist >= 4) {
            set_custom_secrete_message(g_reg_key, "이동");
            if (my_mp >= 1) {
                cmd = safe_blink_against_sniper(mx, my, ex, ey, 0);
                if (cmd) goto END;
            }
            cmd = safe_move_against_sniper(mx, my, ex, ey, 0);
            goto END;
        }
    }

    // ========================================================
    // 생존
    // ========================================================
    if (my_hp <= 2) {
        need_rig = 1;
        if (my_mp >= 2 && poison_active) {
            cmd = CMD_BLESS;
            set_custom_secrete_message(g_reg_key, "축복으로 독을 정화!");
        }
        else if (my_mp >= 1) {
            cmd = CMD_HEAL;
            set_custom_secrete_message(g_reg_key, "위험해! 즉시 회복한다!");
        }
        else {
            cmd = CMD_REST;
            set_custom_secrete_message(g_reg_key, "기모으기");
        }
        goto END;
    }

    // ========================================================
    // MP 회복
    // ========================================================
    if (my_mp <= 1 && dist >= 3) {
        need_rig = 1;
        cmd = CMD_REST;
        set_custom_secrete_message(g_reg_key, "숨고르기");
        goto END;
    }

    // ========================================================
    // 기본 접근
    // ========================================================
    cmd = safe_move_against_sniper(mx, my, ex, ey, 0);
    set_custom_secrete_message(g_reg_key, "이동");

END:
    if (need_rig && my_id == 2)
        rig_initiative_for_me(my_id);
    return cmd;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/* -------------------------------------------------------
   PlayerB 초기 등록 + 스킬 해금 시도
------------------------------------------------------- */

void student1_ai_entry() {
    static int inited = 0;
    if (inited) return;
    inited = 1;

    /* AI 등록 */
    g_reg_key = register_player_ai("작무개김", student2_ai);

    /* 스킬 정답 계산 */
    char s6[256] = { 0 }, key8[128], s13[64] = { 0 }, s14[64] = { 0 }, s16[64] = { 0 }, s19[128] = { 0 };

    solve_skill_6(s6, sizeof(s6));
    solve_skill_7();
    solve_skill_8_11(key8);
    solve_skill_13(s13, sizeof(s13));
    solve_skill_14(s14, sizeof(s14));
    solve_skill_16(s16, sizeof(s16));
    solve_skill_17_18();
    solve_skill_19(s19, sizeof(s19));


    /* 실제 해금 */
    attempt_skill_unlock(g_reg_key, CMD_POISON, s6);

    attempt_skill_unlock(g_reg_key, CMD_STRIKE, s7);

    attempt_skill_unlock(g_reg_key, CMD_BLINK_UP, key8);

    attempt_skill_unlock(g_reg_key, CMD_HEAL_ALL, s13);

    attempt_skill_unlock(g_reg_key, CMD_RANGE_ATTACK, s14);

    attempt_skill_unlock(g_reg_key, CMD_BLESS, s16);

    attempt_skill_unlock(g_reg_key, CMD_H_ATTACK, s17);

    attempt_skill_unlock(g_reg_key, CMD_SECRETE, s19);

    //해금됐는지 확인하려면 아래와 같이 is_skill_unlocked 사용해볼것!!!
    if (is_skill_unlocked(g_reg_key, CMD_POISON))
        printf("작무개김 : 1번 문제 CMD_POISON 해금 완료\n");
    else
        printf("작무개김 : 1번 문제 CMD_POISON 해금 실패 ㅜㅜ\n");


    if (is_skill_unlocked(g_reg_key, CMD_STRIKE))
        printf("작무개김 : 2번 문제 CMD_STRIKE 해금 완료\n");
    else
        printf("작무개김 : 2번 문제 CMD_STRIKE 해금 실패 ㅜㅜ\n");



    if (is_skill_unlocked(g_reg_key, CMD_BLINK_DOWN))
        printf("작무개김 : 3번 문제 CMD_BLINK 4종 해금 완료\n");
    else
        printf("작무개김 : 3번 문제 CMD_BLINK 4종 해금 실패 ㅜㅜ\n");



    if (is_skill_unlocked(g_reg_key, CMD_HEAL_ALL))
        printf("작무개김 : 4번 문제 CMD_HEAL_ALL 해금 완료\n");
    else
        printf("작무개김 : 4번 문제 CMD_HEAL_ALL 해금 실패 ㅜㅜ\n");


    if (is_skill_unlocked(g_reg_key, CMD_RANGE_ATTACK))
        printf("작무개김 : 5번 문제 CMD_RANGE_ATTACK 해금 완료\n");
    else
        printf("작무개김 : 5번 문제 CMD_RANGE_ATTACK 해금 실패 ㅜㅜ\n");


    if (is_skill_unlocked(g_reg_key, CMD_BLESS))
        printf("작무개김 : 6번 문제 CMD_BLESS 해금 완료\n");
    else
        printf("작무개김 : 6번 문제 CMD_BLESS 해금 실패 ㅜㅜ\n");


    if (is_skill_unlocked(g_reg_key, CMD_H_ATTACK))
        printf("작무개김 : 7번 문제 CMD_H_ATTACK,CMD_V_ATTACK  해금 완료\n");
    else
        printf("작무개김 : 7번 문제 CMD_H_ATTACK,CMD_V_ATTACK 해금 실패 ㅜㅜ\n");

    // CMD_SECRETE (비밀 메시지) 해금 및 설정 예시

    if (is_skill_unlocked(g_reg_key, CMD_SECRETE))
    {
        printf("작무개김 : 8번 문제 CMD_SECRETE 해금 완료\n");
        // set_custom_secrete_message 함수를 사용하여 도발 메시지를 등록합니다.
        set_custom_secrete_message(g_reg_key, "후후후 좁밥들...");
    }
    else
        printf("작무개김 : 8번 문제 CMD_SECRETE 해금 실패 ㅜㅜ\n");

    // ------------------------------------------------------------------

    printf("작무개김 : 플레이어 초기화 완료. 아무키나 누르시오.\n");

    // getchar()는 그냥 멈추려고 사용한거에요
    getchar();
}
