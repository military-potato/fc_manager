// =============================================================
// Player.cpp
// Player 클래스와 관련 함수들의 실제 구현 파일.
//
// 주요 내용:
//   - 역할 이름 변환 (roleToString)
//   - 선수 생성 및 스탯 초기화 (Player 생성자)
//   - 선수 정보 출력 (printInfo)
//   - 랜덤 이름/선수 생성 함수
// =============================================================

#include "Player.h"   // Player 클래스, Role 열거형 선언
#include "Utils.h"    // randInt() 함수
#include "NamePool.h" // FIRST_NAMES, LAST_NAMES 배열
#include <iostream>   // std::cout
#include <iomanip>    // std::setw(), std::left (출력 정렬)


// -------------------------------------------------------------
// roleToString()
// Role 열거형 값을 한글 문자열로 변환한다.
// -------------------------------------------------------------
std::string roleToString(Role r) {
    switch (r) {
        case Role::DEFENDER:   return "수비수";    // DEFENDER -> "수비수"
        case Role::MIDFIELDER: return "미드필더";  // MIDFIELDER -> "미드필더"
        case Role::FORWARD:    return "공격수";    // FORWARD -> "공격수"
    }
    return "?";  // 혹시 모를 예외 상황 (정상적으로는 여기 오지 않음)
}


// -------------------------------------------------------------
// Player 생성자
// 이름을 받아서 선수를 초기화한다.
//
// 초기화 목록(콜론 뒤):
//   name(n)              - 전달받은 이름 n을 name에 저장
//   role(Role::MIDFIELDER) - 역할 기본값은 미드필더
//                           applyFormation()이 호출되면 실제 역할로 바뀐다.
// -------------------------------------------------------------
Player::Player(const std::string& n)
    : name(n), role(Role::MIDFIELDER)  // 이름 저장, 기본 역할은 미드필더로 설정
{
    // 기획서 6.1: 각 스탯은 0~100 사이 무작위 정수
    attackStat = randInt(100);  // 공격 스탯: 0~100 랜덤
    midStat    = randInt(100);  // 미드 스탯: 0~100 랜덤
    defStat    = randInt(100);  // 수비 스탯: 0~100 랜덤
}


// -------------------------------------------------------------
// printInfo()
// 선수 한 명의 정보를 콘솔에 한 줄로 출력한다.
//
// std::left: 이후 출력을 왼쪽 정렬로 설정
// std::setw(22): 다음 출력 항목의 너비를 22칸으로 설정 (이름 열 너비 고정)
//                이름 길이가 달라도 다음 열이 같은 위치에서 시작되어 표처럼 보임
//
// 출력 예시:
//   Marcus Silva           | 공격: 72  미드: 45  수비: 88  [수비수]
// -------------------------------------------------------------
void Player::printInfo() const {
    std::cout << std::left << std::setw(22) << name       // 이름: 왼쪽 정렬, 22칸 고정
              << " | 공격:" << std::setw(4) << attackStat // 공격 스탯: 4칸
              << " 미드:" << std::setw(4) << midStat      // 미드 스탯: 4칸
              << " 수비:" << std::setw(4) << defStat      // 수비 스탯: 4칸
              << " [" << roleToString(role) << "]\n";     // 현재 역할 (예: [수비수])
}


// -------------------------------------------------------------
// generateRandomName()
// FIRST_NAMES와 LAST_NAMES 배열에서 각각 랜덤으로 하나씩 골라
// "이름 성" 형식으로 합쳐서 반환한다.
//
// (int)FIRST_NAMES.size() - 1:
//   size()가 반환하는 타입은 부호 없는 정수(size_t)인데,
//   randInt()는 int를 받으므로 (int)로 형변환한다.
//   -1을 하는 이유는 배열 인덱스가 0부터 시작하기 때문.
//   예: 배열 크기가 50이면 유효 인덱스는 0~49이므로 randInt(49)를 써야 함.
// -------------------------------------------------------------
std::string generateRandomName() {
    int fi = randInt((int)FIRST_NAMES.size() - 1);  // 이름 인덱스: 0 ~ 49 랜덤
    int li = randInt((int)LAST_NAMES.size() - 1);   // 성 인덱스:  0 ~ 49 랜덤
    return FIRST_NAMES[fi] + " " + LAST_NAMES[li];  // "이름 성" 형식으로 합치기
}


// -------------------------------------------------------------
// generateRandomPlayer()
// 랜덤 이름을 생성한 뒤 그 이름으로 Player 객체를 만들어 반환한다.
// Team 생성자(10명 자동 배정)와 트레이드 후보 선수 생성 시 사용된다.
// -------------------------------------------------------------
Player generateRandomPlayer() {
    return Player(generateRandomName());  // 랜덤 이름으로 Player 생성 후 반환
}
