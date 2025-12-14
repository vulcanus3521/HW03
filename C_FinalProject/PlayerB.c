/*
 * =================================================================================================
    코드 하나로 합친 사람 : 정진욱
    AI 제작 : 정진욱

    1, 8번 문제 : 정진욱
    2, 7번 문제 : 김용환
    3, 4번 문제 : 김한슬
    5, 6번 문제 : 보류 (정형욱)
 * =================================================================================================
 */

#define _CRT_SECURE_NO_WARNINGS
#include "api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    FILE* fp = fopen("AI1-2_C_Final.csv", "r");
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
    FILE* fp = fopen("AI1-2_C_Final.csv", "r");
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
   문제 5 (원거리공격 해금, 스킬 14 / CMD_RANGE_ATTACK)
   - KEY_FRAG == "K" 인 아이템을 찾아 그 아이템의 HP를 N으로 정의
   - 파일의 시작(오프셋 0)에서 N 바이트 앞으로 fseek 이동
   - 그 위치에서 5바이트를 읽어 앞뒤로 따옴표를 붙여 정답 생성
     (출력/전달용 문자열에는 실제 큰따옴표 문자를 포함)
------------------------------------------------------- */
void solve_skill_14(char* out, int outlen)
{
    out[0] = '\0';

    Item items[256];
    int n = load_csv(items, 256);

    /* 1. KEY_FRAG에 'K'가 들어있는 아이템 찾기 */
    int found_id = -1;
    int hp_value = 0;
    char keyfrag[64] = { 0 };

    for (int i = 0; i < n; i++) {
        if (strchr(items[i].key_frag, 'K') != NULL) {
            found_id = items[i].id;
            hp_value = items[i].hp;
            strcpy(keyfrag, items[i].key_frag);

            //printf("KEY_FRAG = %s → ID %d\n", keyfrag, found_id);
            //printf("HP = %d\n", hp_value);
            break;
        }
    }

    if (found_id == -1) {
        printf("[ERROR] KEY_FRAG 안에 'K' 포함된 아이템 없음\n");
        return;
    }

    /* 2. BIN 파일 열기 */
    FILE* fp = fopen("AI1-2_C_Final.csv", "rb");
    if (!fp) {
        printf("[ERROR] AI1-2_C_Final.csv 파일 열기 실패!\n");
        return;
    }

    /* 3. fseek 이동 */
    //printf("fseek(0, %d, SEEK_SET)\n", hp_value);
    fseek(fp, hp_value - 1, SEEK_SET);

    /* 4. 5글자 읽기 */
    char buf[6] = { 0 };
    fread(buf, 1, 5, fp);
    fclose(fp);

    //printf("파일 %d번째 바이트부터 읽은 5글자: %s\n", hp_value, buf);

    /* 5. 결과를 \"문자열\" 형태로 변환 */
    snprintf(out, outlen, "\"%s\"", buf);

    //printf("정답: \"%s\"\n", buf);
}

/* -------------------------------------------------------
   문제 6 (자폭 스킬, CMD_SELF_DESTRUCT = 16)
   - NAME에 "Sword" 포함 아이템 찾기
   - 해당 아이템들의 KEY_FRAG를 순서대로 이어붙여 S 생성
   - S를 '*' 기준으로 strtok → 토큰 중 가장 긴 것 선택
   - 길이 같으면 먼저 등장한 토큰
   - 모든 과정 출력
------------------------------------------------------- */
void solve_skill_16(char* out, int outlen)
{
    out[0] = '\0';

    Item items[256];
    int n = load_csv(items, 256);

    char S[256] = { 0 };
    int found_count = 0;

    //printf("=== [자폭 스킬 해금 풀이 과정] ===\n");

    /* 1. NAME에 Sword 포함 아이템 찾기 */
    for (int i = 0; i < n; i++) {
        if (strstr(items[i].name, "Sword") != NULL) {
            //printf("Sword 포함: ID %d (%s), KEY_FRAG=%s\n", items[i].id, items[i].name, items[i].key_frag);

            strncat(S, items[i].key_frag, sizeof(S) - strlen(S) - 1);
            found_count++;
        }
    }

    if (found_count == 0) {
        printf("[ERROR] Sword 포함 아이템 없음\n");
        return;
    }

    //printf("S = \"%s\"\n", S);

    /* 2. strtok('*') → 토큰 중 가장 긴 것 선택 */
    char S_copy[256];
    strcpy(S_copy, S);

    char* token = strtok(S_copy, "*");
    char best[128] = { 0 };

    int t_index = 0;
    while (token != NULL) {
        //printf("토큰 %d = \"%s\" (길이=%d)\n", t_index, token, (int)strlen(token));

        if (strlen(token) > strlen(best)) {
            strcpy(best, token);
            //printf(" → 현재 최장 토큰 갱신: \"%s\"\n", best);
        }
        /* 길이 동일하면 '먼저 나온 토큰 유지' 규칙이므로 갱신 X */

        t_index++;
        token = strtok(NULL, "*");
    }

    /* 3. 정답 반환 */
    strncpy(out, best, outlen - 1);
    out[outlen - 1] = '\0';

    //printf("최종 선택된 토큰 = \"%s\"\n", out);
    //printf("=== [자폭 스킬 풀이 끝] ===\n");
}

/* -------------------------------------------------------
   문제 7 - 김용환, s7 전역 변수에 정답 저장
------------------------------------------------------- */
char s17[64] = { 0 };
void solve_skill_17_18(void)
{
    FILE* fp = fopen("AI1-2_C_Final.csv", "r");
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


// ---------------- 거리 계산 ----------------
static int calculate_distance(const Player* p1, const Player* p2) {
    int dx = abs(get_player_x(p1) - get_player_x(p2));
    int dy = abs(get_player_y(p1) - get_player_y(p2));
    return dx + dy;
}

// ---------------- 맵 정의 ----------------
#define MAP_SIZE 7
#define MIN_COORD 0
#define MAX_COORD (MAP_SIZE - 1)


extern void set_custom_secrete_message(const char* key, const char* message);

// ---------------- AI ----------------
int student2_ai(const Player* my_info, const Player* opponent_info) {
    int my_hp = get_player_hp(my_info);
    int my_mp = get_player_mp(my_info);
    int my_x = get_player_x(my_info);
    int my_y = get_player_y(my_info);

    int opp_x = get_player_x(opponent_info);
    int opp_y = get_player_y(opponent_info);

    int distance = calculate_distance(my_info, opponent_info);
    int opp_last = get_player_last_command(opponent_info);

    // ---------- 행동 감지 ----------
    static int last_cmd = -1;
    static int repeat_cnt = 0;

    // ---------- 첫 턴 ----------
    static int first_turn = 1;

    // ----------------------------------
    // 0. 행동 3회 이상 → MP 회복
    //    행동 2회 이상 → 독 공격
    // ----------------------------------
    if (repeat_cnt >= 3) {
        repeat_cnt = 0;
        last_cmd = CMD_REST;
        set_custom_secrete_message(g_reg_key, "잠시 숨을 고르자.");
        return CMD_REST;
    }
    if (repeat_cnt >= 2&& my_mp >= 5) {
        last_cmd = CMD_POISON;
        set_custom_secrete_message(g_reg_key, "독 공격!");
        return CMD_POISON;
    }

    // ----------------------------------
    // 1. 첫 턴은 무조건 독
    // ----------------------------------
    if (first_turn) {
        first_turn = 0;
        last_cmd = CMD_POISON;
        repeat_cnt = 1;
        set_custom_secrete_message(g_reg_key, "독 공격!");
        return CMD_POISON;
    }

    // ----------------------------------
    // 2. 상대가 독 → 축복으로 해제
    // ----------------------------------
    if (opp_last == CMD_POISON && my_mp >= 2) {
        last_cmd = CMD_BLESS;
        repeat_cnt = (last_cmd == CMD_BLESS) ? repeat_cnt + 1 : 1;
        set_custom_secrete_message(g_reg_key, "축복으로 정화!");
        return CMD_BLESS;
    }

    // ----------------------------------
    // 3. HP 위험 → 회복
    // ----------------------------------
    if (my_hp <= 2 && my_mp >= 1) {
        last_cmd = CMD_HEAL;
        repeat_cnt = (last_cmd == CMD_HEAL) ? repeat_cnt + 1 : 1;
        set_custom_secrete_message(g_reg_key, "위험해! 회복한다!");
        return CMD_HEAL;
    }

    // ----------------------------------
    // 4. 근접 전투
    // ----------------------------------
    if (distance <= 1) {
        if (my_mp >= 2) {
            last_cmd = CMD_STRIKE;
            repeat_cnt = (last_cmd == CMD_STRIKE) ? repeat_cnt + 1 : 1;
            set_custom_secrete_message(g_reg_key, "강타다!");
            return CMD_STRIKE;
        }
        else {
            last_cmd = CMD_ATTACK;
            repeat_cnt = (last_cmd == CMD_ATTACK) ? repeat_cnt + 1 : 1;
            set_custom_secrete_message(g_reg_key, "기본 공격!");
            return CMD_ATTACK;
        }
    }

    // ----------------------------------
    // 5. MP 부족 → 휴식
    // ----------------------------------
    if (my_mp <= 2) {
        last_cmd = CMD_REST;
        repeat_cnt = (last_cmd == CMD_REST) ? repeat_cnt + 1 : 1;
        set_custom_secrete_message(g_reg_key, "MP 보충!");
        return CMD_REST;
    }

    // ----------------------------------
    // 6. 거리 벌리기 (점멸 우선)
    // ----------------------------------
    if (distance < 4 && my_mp >= 1) {
        // Y축 회피
        if (my_y < opp_y && my_y > MIN_COORD) {
            last_cmd = CMD_BLINK_UP;
            repeat_cnt = (last_cmd == CMD_BLINK_UP) ? repeat_cnt + 1 : 1;
            set_custom_secrete_message(g_reg_key, "위로 점멸!");
            return CMD_BLINK_UP;
        }
        if (my_y > opp_y && my_y < MAX_COORD) {
            last_cmd = CMD_BLINK_DOWN;
            repeat_cnt = (last_cmd == CMD_BLINK_DOWN) ? repeat_cnt + 1 : 1;
            set_custom_secrete_message(g_reg_key, "아래로 점멸!");
            return CMD_BLINK_DOWN;
        }
        // X축 회피
        if (my_x < opp_x && my_x > MIN_COORD) {
            last_cmd = CMD_BLINK_LEFT;
            repeat_cnt = (last_cmd == CMD_BLINK_LEFT) ? repeat_cnt + 1 : 1;
            set_custom_secrete_message(g_reg_key, "왼쪽으로 점멸!");
            return CMD_BLINK_LEFT;
        }
        if (my_x > opp_x && my_x < MAX_COORD) {
            last_cmd = CMD_BLINK_RIGHT;
            repeat_cnt = (last_cmd == CMD_BLINK_RIGHT) ? repeat_cnt + 1 : 1;
            set_custom_secrete_message(g_reg_key, "오른쪽으로 점멸!");
            return CMD_BLINK_RIGHT;
        }
    }

    // ----------------------------------
    // 7. 추격 이동 (벽 고려)
    // ----------------------------------
    if (my_x < opp_x && my_x < MAX_COORD) {
        last_cmd = CMD_RIGHT;
        repeat_cnt = (last_cmd == CMD_RIGHT) ? repeat_cnt + 1 : 1;
        set_custom_secrete_message(g_reg_key, "오른쪽으로 접근!");
        return CMD_RIGHT;
    }
    if (my_x > opp_x && my_x > MIN_COORD) {
        last_cmd = CMD_LEFT;
        repeat_cnt = (last_cmd == CMD_LEFT) ? repeat_cnt + 1 : 1;
        set_custom_secrete_message(g_reg_key, "왼쪽으로 접근!");
        return CMD_LEFT;
    }
    if (my_y < opp_y && my_y < MAX_COORD) {
        last_cmd = CMD_DOWN;
        repeat_cnt = (last_cmd == CMD_DOWN) ? repeat_cnt + 1 : 1;
        set_custom_secrete_message(g_reg_key, "아래로 이동!");
        return CMD_DOWN;
    }
    if (my_y > opp_y && my_y > MIN_COORD) {
        last_cmd = CMD_UP;
        repeat_cnt = (last_cmd == CMD_UP) ? repeat_cnt + 1 : 1;
        set_custom_secrete_message(g_reg_key, "위로 이동!");
        return CMD_UP;
    }

    // ----------------------------------
    // 8. 예외 처리
    // ----------------------------------
    last_cmd = CMD_ATTACK;
    repeat_cnt = (last_cmd == CMD_ATTACK) ? repeat_cnt + 1 : 1;
    set_custom_secrete_message(g_reg_key, "예외 상황!");
    return CMD_ATTACK;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/* -------------------------------------------------------
   PlayerB 초기 등록 + 스킬 해금 시도
------------------------------------------------------- */

void student2_ai_entry() {
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

    // ------------------------------------------------------------------
// [COMMAND UNLOCK SECTION] 
// 학생은 아래의 코드를 복사하여 필요한 스킬 수만큼 반복해야 합니다.
// ------------------------------------------------------------------

//attempt_skill_unlock(고유번호, 스킬번호, 시험문제 정답) 함수를 실행하여 시험문제의 답을 입력하면 기술이 해금됩니다.


    //주의: 시험문제에서 제공되는 CSV는 데이터를 바꾸어 최종 시험때 실행됩니다.
    //위와 같이 string을 직접 입력하지마시오 시험문제를 풀어서 CSV로부터 저 string을 뽑아내는 코드를 작성해서 해금하시기 바랍니다.
    //그래야 시험 당일날에도 스킬이 정상적으로 해금됩니다.


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
