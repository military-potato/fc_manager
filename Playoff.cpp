// =============================================================
// Playoff.cpp
// 플레이오프 진행 로직의 실제 구현 파일.
//
// 기획서 4.2의 플레이오프 방식을 구현한다.
// 최하위 팀부터 시작해서 한 번 지면 탈락, 마지막까지 살아남은 팀이 우승.
// =============================================================

#include "Playoff.h"  // runPlayoff() 함수 선언
#include "Match.h"    // playoffTiebreak() (승부차기)
#include "Utils.h"    // randInt(), printLine(), waitEnter()
#include <iostream>   // std::cout
#include <algorithm>  // (현재 직접 사용하지 않음)


// =============================================================
// runPlayoff()
// 플레이오프 전체를 진행한다.
//
// 핵심 개념:
//   survivor  - 현재까지 살아남아 다음 경기를 기다리는 팀
//   challenger - survivor에게 도전하는 팀 (순위 위의 팀)
//
// 20위 팀이 첫 survivor가 되어 19위(challenger)와 경기.
// 이기면 계속 survivor로 남고, 지면 challenger가 새 survivor.
// 이것을 1위 팀과의 경기까지 반복하면 최종 우승팀이 결정된다.
// =============================================================
std::string runPlayoff(League& league) {
    printLine();
    std::cout << "*** 플레이오프 시작 ***\n";
    printLine();

    // 리그 순위 순으로 정렬된 팀 인덱스 배열
    // sorted[0] = 1위 팀의 teams 인덱스
    // sorted[19] = 20위 팀의 teams 인덱스
    auto sorted = league.getSortedTeamIndices();

    // 20위 팀(sorted[19])이 첫 번째 survivor
    // survivorLeagueIdx: 현재 살아있는 팀의 league.teams 배열 인덱스
    int survivorLeagueIdx = sorted[19];

    // step 18 (19위 팀) 부터 step 0 (1위 팀) 까지 순서대로 도전
    // step이 클수록 순위가 낮은 팀 (19위는 step 18, 1위는 step 0)
    for (int step = 18; step >= 0; --step) {
        int challengerLeagueIdx = sorted[step];  // 이번에 도전하는 팀의 인덱스

        // 팀 객체를 참조로 가져옴 (참조: 원본 팀 객체를 직접 사용)
        Team& survivor   = league.teams[survivorLeagueIdx];
        Team& challenger = league.teams[challengerLeagueIdx];

        // 경기 정보 출력
        std::cout << "\n[플레이오프] " << survivor.name
                  << " vs " << challenger.name << "\n";

        // 경기 진행
        // 플레이오프 경기는 성적(승점/승무패 기록)에 영향을 주지 않는다.
        // 그래서 playMatch()를 쓰지 않고 직접 득점을 계산한다.
        int sMax   = survivor.calcMaxGoals(challenger);   // survivor 득점 최댓값
        int cMax   = challenger.calcMaxGoals(survivor);   // challenger 득점 최댓값
        int sGoals = randInt(sMax);  // survivor 실제 득점 (0 ~ sMax 랜덤)
        int cGoals = randInt(cMax);  // challenger 실제 득점 (0 ~ cMax 랜덤)

        // 스코어 출력
        std::cout << survivor.name << " " << sGoals
                  << " : " << cGoals << " " << challenger.name << "\n";

        // 승패 판정 및 다음 survivor 결정
        if (sGoals > cGoals) {
            // survivor가 더 많이 넣었으면 survivor 계속 생존
            std::cout << survivor.name << " 진출!\n";
            // survivorLeagueIdx는 변경 없음 (survivor 그대로 유지)
        } else if (sGoals < cGoals) {
            // challenger가 더 많이 넣었으면 challenger가 새 survivor
            std::cout << challenger.name << " 진출!\n";
            survivorLeagueIdx = challengerLeagueIdx;  // survivor 교체
        } else {
            // 득점이 같으면 무승부 -> 승부차기 진행
            // playoffTiebreak()가 true를 반환하면 survivor 승리 (survivorLeagueIdx 유지)
            // false를 반환하면 challenger 승리 (survivorLeagueIdx 교체)
            bool survivorWins = playoffTiebreak(survivor.name, challenger.name);
            if (!survivorWins)
                survivorLeagueIdx = challengerLeagueIdx;  // challenger가 새 survivor
        }

        waitEnter();  // 다음 경기 전에 플레이어가 결과를 확인할 시간
    }

    // 모든 경기가 끝난 후 마지막으로 살아남은 팀이 우승
    std::string champion = league.teams[survivorLeagueIdx].name;

    printLine();
    std::cout << "시즌 최종 우승: " << champion << "\n";
    printLine();

    return champion;  // 우승 팀 이름 반환 (Game.cpp에서 사용)
}
