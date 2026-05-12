// =============================================================
// Match.cpp
// 경기 진행 로직의 실제 구현 파일.
//
// 기획서 6.2.1 ~ 6.2.3을 직접 구현하는 핵심 파일.
// 득점 최댓값 계산 -> 랜덤 득점 뽑기 -> 승무패 판정 -> 성적 업데이트
// 순서로 진행된다.
// =============================================================

#include "Match.h"  // MatchResult, MatchScore, 함수 선언
#include "Utils.h"  // randInt() (랜덤 득점 생성에 사용)
#include <iostream> // std::cout (경기 결과 출력에 사용)


// =============================================================
// playMatch()
// 두 팀이 경기를 진행하고 결과를 반환한다.
//
// 진행 순서:
//   1. 각 팀의 득점 최댓값 계산 (calcMaxGoals)
//   2. [0, 최댓값] 범위에서 랜덤 득점 뽑기
//   3. 두 득점 비교해서 승무패 판정
//   4. (선택) 결과 콘솔 출력
//   5. 두 팀의 성적 데이터 업데이트
// =============================================================
MatchScore playMatch(Team& home, Team& away, bool printDetail) {
    // 1단계: 각 팀의 득점 최댓값 계산
    // calcMaxGoals()는 Team.cpp에 구현되어 있고, 기획서 6.2.3 계산식을 사용한다.
    // home이 away를 상대로, away가 home을 상대로 각각 계산한다.
    int homeMax = home.calcMaxGoals(away);  // 홈 팀의 득점 최댓값
    int awayMax = away.calcMaxGoals(home);  // 어웨이 팀의 득점 최댓값

    // 2단계: [0, 최댓값] 범위에서 랜덤 득점 뽑기
    // 기획서 6.2.1: 범위 안의 모든 정수가 동일한 확률로 뽑힌다.
    // 예: homeMax=4이면 0,1,2,3,4 중 하나가 각 20% 확률로 결정됨
    int homeGoals = randInt(homeMax);  // 홈 팀 득점 (0 ~ homeMax 중 랜덤)
    int awayGoals = randInt(awayMax);  // 어웨이 팀 득점 (0 ~ awayMax 중 랜덤)

    // 3단계: 두 득점을 비교해서 홈 팀 기준 결과 판정
    // 기획서 6.2.1 (3): 더 큰 득점의 팀이 승리, 같으면 무승부
    MatchResult res;
    if (homeGoals > awayGoals)      res = MatchResult::WIN;   // 홈 팀 득점이 더 많으면 홈 승
    else if (homeGoals < awayGoals) res = MatchResult::LOSS;  // 어웨이 득점이 더 많으면 홈 패
    else                            res = MatchResult::DRAW;  // 같으면 무승부

    // 4단계: 플레이어 경기일 때만 결과를 콘솔에 출력
    // AI끼리 경기는 printDetail=false이므로 이 블록이 실행되지 않는다.
    if (printDetail) {
        std::cout << "\n[경기 결과]\n";
        // 예: "내팀 2 : 1 Red Dragons FC"
        std::cout << home.name << " " << homeGoals
                  << " : " << awayGoals << " " << away.name << "\n";
    }

    // 5단계: 두 팀의 누적 성적 데이터 업데이트
    // 득실 기록: 홈/어웨이 각각 득점은 +, 실점은 상대 득점
    home.goalsFor     += homeGoals;  // 홈 팀 누적 득점에 이번 경기 득점 추가
    home.goalsAgainst += awayGoals;  // 홈 팀 누적 실점에 어웨이 득점 추가
    away.goalsFor     += awayGoals;  // 어웨이 팀 누적 득점
    away.goalsAgainst += homeGoals;  // 어웨이 팀 누적 실점

    // 승점 및 승/무/패 카운트 업데이트
    // 기획서 6.7.2: 승리 시 3.001을 더한다 (3이 아닌 이유: 같은 승점일 때 승수로 자동 정렬)
    if (res == MatchResult::WIN) {
        home.points += 3.001;  // 홈 팀 승리: 승점 3.001 추가
        home.wins++;           // 홈 팀 승리 수 1 증가
        away.losses++;         // 어웨이 팀 패배 수 1 증가
    } else if (res == MatchResult::LOSS) {
        away.points += 3.001;  // 어웨이 팀 승리: 어웨이에 승점 3.001 추가
        away.wins++;           // 어웨이 팀 승리 수 1 증가
        home.losses++;         // 홈 팀 패배 수 1 증가
    } else {
        home.points += 1.0;  // 무승부: 양 팀 모두 승점 1 추가
        home.draws++;        // 홈 팀 무승부 수 1 증가
        away.points += 1.0;  // 어웨이 팀도 승점 1
        away.draws++;        // 어웨이 팀 무승부 수 1 증가
    }

    // 결과 구조체를 만들어서 반환
    // {homeGoals, awayGoals, res}: MatchScore 구조체의 멤버를 순서대로 초기화
    return {homeGoals, awayGoals, res};
}


// =============================================================
// playoffTiebreak()
// 플레이오프에서 무승부가 나왔을 때 50% 확률로 승자를 결정한다.
// 기획서 6.7.1 (4): 두 팀 모두 승리 확률이 50%
//
// randInt(1): 0 또는 1을 50% 확률로 반환한다.
//   0이 나오면 teamA(survivor) 승리
//   1이 나오면 teamB(challenger) 승리
// =============================================================
bool playoffTiebreak(const std::string& teamA, const std::string& teamB) {
    int r = randInt(1);  // 0 또는 1을 랜덤으로 뽑음 (각 50% 확률)

    std::cout << "[승부차기] " << teamA << " vs " << teamB << " : ";

    if (r == 0) {
        std::cout << teamA << " 승리!\n";  // 0이 나오면 teamA 승
        return true;                       // true = teamA 승리
    } else {
        std::cout << teamB << " 승리!\n";  // 1이 나오면 teamB 승
        return false;                      // false = teamB 승리 (teamA 패배)
    }
}
