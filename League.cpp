// =============================================================
// League.cpp
// League 클래스의 실제 구현 파일.
//
// 기획서 4.1: 20팀, 2라운드 리그, 팀당 38경기
// 기획서 6.5: 승점제 (승리 3점, 무승부 1점, 패배 0점)
// 기획서 6.7.2: 승리 시 실제로는 3.001점을 줘서 승수 정렬 처리
// =============================================================

#include "League.h"   // League 클래스, Fixture 구조체
#include "Match.h"    // playMatch() 함수
#include "Utils.h"    // randInt(), printLine()
#include "NamePool.h" // TEAM_NAMES 배열 (AI 팀 이름)
#include <iostream>   // std::cout
#include <iomanip>    // std::setw() (순위표 정렬)
#include <algorithm>  // std::sort() (순위 정렬에 사용)


// =============================================================
// League 기본 생성자
// 멤버 변수를 초기값으로 설정한다.
// 실제 팀/일정 생성은 init()에서 따로 한다.
// init()을 별도로 분리한 이유: 새 시즌 시작 시 같은 객체를 재사용할 수 있어서
// =============================================================
League::League()
    : currentFixtureIdx(0), leagueFinished(false) {}  // 진행 인덱스 0, 미완료 상태로 시작


// =============================================================
// init()
// 리그를 새로 초기화한다. 게임 시작 또는 새 시즌 시작 시 호출된다.
// =============================================================
void League::init(const std::string& playerTeamName) {
    // 기존 데이터를 모두 비운다 (새 시즌 시작 시 이전 시즌 데이터 제거)
    teams.clear();     // 팀 목록 초기화
    fixtures.clear();  // 경기 일정 초기화
    currentFixtureIdx = 0;      // 경기 진행 인덱스 초기화
    leagueFinished    = false;  // 리그 완료 여부 초기화

    // 플레이어 팀 생성 (항상 teams[0]에 위치)
    // emplace_back: 객체를 teams 배열 끝에 직접 생성해서 추가 (복사 없이 효율적)
    teams.emplace_back(playerTeamName);

    // AI 팀 19개 생성
    for (int i = 0; i < 19; ++i) {
        Team t(TEAM_NAMES[i]);  // NamePool.h의 팀 이름 배열에서 순서대로 이름을 가져옴

        // AI 팀의 포메이션: 0~9 중 랜덤 선택
        t.formationIndex = randInt(9);

        // AI 팀의 전술: 0/1/2 중 랜덤 선택 후 Tactic 열거형으로 변환
        int tacticRand = randInt(2);
        if (tacticRand == 0)      t.tactic = Tactic::ATTACK;   // 0 -> 공격 전술
        else if (tacticRand == 1) t.tactic = Tactic::BALANCE;  // 1 -> 균형 전술
        else                      t.tactic = Tactic::DEFENSE;  // 2 -> 수비 전술

        // 선택된 포메이션에 맞게 선수 역할을 배정
        t.applyFormation();

        // std::move: t 객체를 복사하지 않고 teams로 이동시킨다. 성능 최적화.
        teams.push_back(std::move(t));
    }

    // 2라운드 리그 일정 생성
    // 1라운드: 20팀 중 모든 (홈, 어웨이) 조합을 한 번씩
    // i < j 조건으로 같은 쌍이 두 번 생기지 않게 함
    for (int i = 0; i < 20; ++i)
        for (int j = i + 1; j < 20; ++j)
            fixtures.push_back({i, j, false});  // {홈인덱스, 어웨이인덱스, 미진행}
    // 1라운드 결과: 20*19/2 = 190경기

    // 2라운드: 1라운드의 홈/어웨이를 반전해서 추가
    // 1라운드에서 홈이었던 팀이 이번엔 어웨이가 되고 그 반대도 마찬가지
    int round1Size = (int)fixtures.size();  // 1라운드 경기 수 (190)
    for (int k = 0; k < round1Size; ++k)
        fixtures.push_back({fixtures[k].awayIdx, fixtures[k].homeIdx, false});
    // 최종 fixtures 크기: 380경기 (플레이어 관련 경기는 그 중 38개)
}


// =============================================================
// nextPlayerFixtureIdx()
// 아직 진행하지 않은 플레이어 경기 중 가장 가까운 것의 인덱스를 반환한다.
// "플레이어 경기"란 homeIdx == 0 또는 awayIdx == 0 인 경기를 말한다.
// (teams[0]이 플레이어 팀이므로 인덱스 0이 포함된 경기)
// =============================================================
int League::nextPlayerFixtureIdx() const {
    // currentFixtureIdx부터 탐색 시작 (이미 지난 경기는 건너뜀)
    for (int i = currentFixtureIdx; i < (int)fixtures.size(); ++i) {
        const auto& f = fixtures[i];  // auto: 컴파일러가 타입(Fixture)을 자동 추론
        // 아직 진행 안 했고, 플레이어 팀(인덱스 0)이 참여하는 경기이면
        if (!f.played && (f.homeIdx == 0 || f.awayIdx == 0))
            return i;  // 해당 경기의 인덱스 반환
    }
    return -1;  // 플레이어 경기가 더 없음
}


// =============================================================
// nextOpponentName()
// 다음 플레이어 경기의 상대 팀 이름을 반환한다.
// =============================================================
std::string League::nextOpponentName() const {
    int idx = nextPlayerFixtureIdx();  // 다음 플레이어 경기 인덱스
    if (idx < 0) return "";           // 경기가 없으면 빈 문자열 반환

    const auto& f = fixtures[idx];
    // 플레이어가 홈이면 어웨이가 상대, 플레이어가 어웨이면 홈이 상대
    int oppIdx = (f.homeIdx == 0) ? f.awayIdx : f.homeIdx;
    return teams[oppIdx].name;  // 상대 팀 이름 반환
}


// =============================================================
// simulateAIMatches()
// currentFixtureIdx부터 upToFixtureIdx 직전까지의 경기 중
// 플레이어가 참여하지 않는 AI끼리의 경기를 자동으로 진행한다.
//
// 호출 시점: 플레이어 경기 직전에 그 이전 AI 경기들을 처리할 때
// =============================================================
void League::simulateAIMatches(int upToFixtureIdx) {
    for (int i = currentFixtureIdx; i < upToFixtureIdx && i < (int)fixtures.size(); ++i) {
        auto& f = fixtures[i];  // 현재 경기 일정 참조 (수정 가능하도록 & 사용)

        // 아직 진행 안 됐고, 양 팀 모두 플레이어 팀(0)이 아닌 경기만 처리
        if (!f.played && f.homeIdx != 0 && f.awayIdx != 0) {
            // AI 경기는 결과를 출력하지 않음 (printDetail = false)
            playMatch(teams[f.homeIdx], teams[f.awayIdx], false);
            f.played = true;  // 경기 완료 표시
        }
    }
}


// =============================================================
// playPlayerMatch()
// 플레이어의 다음 경기를 진행한다.
//
// 진행 순서:
//   1. 다음 플레이어 경기 인덱스 찾기
//   2. 그 이전의 AI 경기 자동 처리
//   3. 플레이어 경기 진행 (결과 출력 포함)
//   4. 플레이어가 어웨이이면 결과를 플레이어 기준으로 반전
//   5. 경기 완료 표시 및 인덱스 업데이트
//   6. 마지막 경기였으면 남은 AI 경기 전부 처리 후 리그 종료
// =============================================================
MatchResult League::playPlayerMatch(Tactic tactic) {
    int idx = nextPlayerFixtureIdx();  // 다음 플레이어 경기 인덱스
    if (idx < 0) return MatchResult::DRAW;  // 경기가 없으면 그냥 반환 (비정상 상황)

    // 이번 플레이어 경기 이전에 있는 AI끼리의 경기들을 먼저 처리
    simulateAIMatches(idx);

    auto& f = fixtures[idx];  // 이번 경기 일정

    // 플레이어가 홈인지 어웨이인지 판단
    bool playerIsHome = (f.homeIdx == 0);

    Team& playerTeam = teams[0];                                     // 플레이어 팀
    Team& oppTeam    = teams[playerIsHome ? f.awayIdx : f.homeIdx];  // 상대 팀

    // 플레이어가 선택한 전술을 팀에 적용
    playerTeam.tactic = tactic;

    // 경기 진행 (결과 출력 포함)
    MatchScore score;
    if (playerIsHome) {
        // 플레이어가 홈이면 그대로 진행
        score = playMatch(playerTeam, oppTeam, true);
    } else {
        // 플레이어가 어웨이이면 홈/어웨이 순서를 맞춰서 진행
        score = playMatch(oppTeam, playerTeam, true);
        // playMatch는 홈 팀 기준으로 결과를 반환하므로, 플레이어 기준으로 반전
        if (score.resultForHome == MatchResult::WIN)
            score.resultForHome = MatchResult::LOSS;   // 홈(상대)이 이겼으면 플레이어는 패배
        else if (score.resultForHome == MatchResult::LOSS)
            score.resultForHome = MatchResult::WIN;    // 홈(상대)이 졌으면 플레이어는 승리
        // DRAW는 반전 없이 그대로
    }

    f.played = true;           // 이번 경기 완료 표시
    currentFixtureIdx = idx + 1;  // 다음 처리할 경기 인덱스를 한 칸 앞으로

    // 플레이어 경기가 더 이상 없으면 (38경기 모두 완료)
    if (nextPlayerFixtureIdx() < 0) {
        // 남아있는 AI 경기를 전부 처리
        simulateAIMatches((int)fixtures.size());
        leagueFinished = true;  // 리그 완료 표시 -> 메인 루프에서 플레이오프로 넘어감
    }

    return score.resultForHome;  // 플레이어 기준 결과 반환 (WIN/DRAW/LOSS)
}


// =============================================================
// getSortedTeamIndices()
// 승점 내림차순으로 정렬된 팀 인덱스 배열을 반환한다.
// 기획서 6.7.2: points는 소수점 포함이므로 승수가 많은 팀이 자동으로 앞에 온다.
// =============================================================
std::vector<int> League::getSortedTeamIndices() const {
    std::vector<int> indices(teams.size());  // 팀 수만큼 인덱스 배열 생성

    // 0, 1, 2, ..., 19 순서로 초기화
    for (int i = 0; i < (int)teams.size(); ++i) indices[i] = i;

    // 람다 함수를 비교 기준으로 사용해서 정렬
    // [this]: 람다 안에서 League의 멤버(teams)에 접근하기 위해 캡처
    // a, b: 비교할 두 팀의 인덱스
    // teams[a].points > teams[b].points: a팀 승점이 높으면 앞에 오도록 (내림차순)
    std::sort(indices.begin(), indices.end(), [this](int a, int b) {
        return teams[a].points > teams[b].points;
    });

    return indices;  // 정렬된 인덱스 배열 반환
}


// =============================================================
// printStandings()
// 현재 리그 순위표를 콘솔에 출력한다.
// 플레이어 팀 앞에는 ">" 마커를 붙여서 구분한다.
// =============================================================
void League::printStandings() const {
    auto sorted = getSortedTeamIndices();  // 승점 순으로 정렬된 팀 인덱스

    printLine();
    std::cout << "순위  팀명                    승   무   패  득실  승점\n";
    printLine();

    for (int r = 0; r < (int)sorted.size(); ++r) {
        int idx = sorted[r];  // r번째 순위 팀의 teams 배열 인덱스

        // 플레이어 팀(인덱스 0)이면 ">" 마커 출력, 아니면 공백
        if (idx == 0) std::cout << ">";
        else          std::cout << " ";

        // 해당 팀의 순위표 한 줄 출력 (Team::printStandingRow)
        teams[idx].printStandingRow(r + 1);  // r+1: 순위는 1부터 시작
    }

    printLine();
}


// =============================================================
// isFinished()
// 리그가 완전히 끝났는지 반환한다.
// =============================================================
bool League::isFinished() const {
    return leagueFinished;  // true이면 리그 종료
}


// =============================================================
// playedMatches()
// 플레이어가 지금까지 진행한 경기 수를 센다.
// fixtures 전체를 순회하면서 플레이어가 참여했고 완료된 경기를 카운트.
// =============================================================
int League::playedMatches() const {
    int cnt = 0;
    for (const auto& f : fixtures)
        // played가 true이고, 플레이어 팀(0)이 홈 또는 어웨이인 경기
        if (f.played && (f.homeIdx == 0 || f.awayIdx == 0)) cnt++;
    return cnt;
}


// =============================================================
// totalMatches()
// 플레이어가 리그에서 치러야 할 총 경기 수를 반환한다.
// 기획서 4.1: 19팀 * 2라운드 = 38경기
// =============================================================
int League::totalMatches() const {
    return 38;
}


// =============================================================
// playerRank()
// 현재 순위표에서 플레이어 팀(teams[0])의 순위를 반환한다.
// =============================================================
int League::playerRank() const {
    auto sorted = getSortedTeamIndices();  // 승점 순 정렬된 인덱스

    // 정렬된 배열에서 인덱스 0(플레이어 팀)이 몇 번째에 있는지 찾음
    for (int r = 0; r < (int)sorted.size(); ++r)
        if (sorted[r] == 0) return r + 1;  // r은 0부터 시작하므로 +1해서 순위 반환

    return -1;  // 정상적으로는 여기까지 오지 않음
}
