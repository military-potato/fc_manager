// =============================================================
// Game.cpp
// Game 클래스의 실제 구현 파일.
// 게임의 전체 흐름(시작, 메인 루프, 각 메뉴 처리)을 담당한다.
// =============================================================

#include "Game.h"     // Game 클래스 선언
#include "Trade.h"    // runTrades() (트레이드 진행)
#include "Playoff.h"  // runPlayoff() (플레이오프 진행)
#include "Utils.h"    // printLine(), waitEnter()
#include <iostream>   // std::cout, std::cin
#include <string>     // std::string, std::getline()
#include <algorithm>  // (현재 직접 사용하지 않음)
#include <cctype>     // tolower() (영어 소문자 변환에 사용)


// =============================================================
// toLower() - 내부 함수
// 문자열의 영어 대문자를 소문자로 변환한다.
// 영어 명령어(schedule, roster 등)를 대소문자 구분 없이 처리하기 위해 사용.
//
// 예: "Schedule" -> "schedule", "ROSTER" -> "roster"
//
// 주의: 한글 문자는 건드리지 않는다.
//       tolower()에 한글 바이트를 넣으면 깨질 수 있으므로
//       (unsigned char)로 변환해서 ASCII 범위(0~127)만 처리한다.
//
// static: 이 파일 안에서만 사용하는 내부 함수
// =============================================================
static std::string toLower(std::string s) {
    for (auto& c : s)                       // 문자열의 각 문자를 순회
        c = (char)tolower((unsigned char)c); // ASCII 문자만 소문자로 변환
    return s;
}

int tradeCount = 0;


// =============================================================
// Game 기본 생성자
// 멤버 변수를 초기값으로 설정한다.
//   lastMatchResult = DRAW: 게임 시작 전이라 아직 경기가 없으므로 임의로 DRAW
//   hasPlayedMatch  = false: 아직 경기를 진행하지 않은 상태
// =============================================================
Game::Game()
    : lastMatchResult(MatchResult::DRAW), hasPlayedMatch(false) {}


// =============================================================
// start()
// 게임 시작 처리. 타이틀 출력, 팀 이름 입력, 리그 초기화.
// =============================================================
void Game::start() {
    printLine();
    std::cout << "  축구 매니저 게임\n";
    std::cout << "눈떠보니 히딩크가 된 당신, 리그 우승을 노려보세요!\n";
    printLine();

    // 팀 이름 입력받기
    std::cout << "팀 이름을 입력하세요: ";
    std::getline(std::cin, playerTeamName);  // 한 줄 전체를 팀 이름으로 받음

    // 아무것도 입력하지 않으면 기본 이름 사용
    if (playerTeamName.empty()) playerTeamName = "My Team";

    // 리그 초기화: 플레이어 팀 + AI 팀 19개 생성, 경기 일정 380개 생성
    league.init(playerTeamName);

    std::cout << "\n[" << playerTeamName << "] 창단 완료!\n";
    std::cout << "선수 10명이 자동으로 배정되었습니다.\n";
    waitEnter();
}



// =============================================================
// run()
// 게임의 메인 루프. start()로 초기화 후 명령어를 반복해서 처리한다.
// "종료" 명령어가 입력되거나 새 시즌을 시작하지 않으면 루프가 끝난다.
// =============================================================
void Game::run() {
    start();  // 게임 초기화 (타이틀, 팀 이름 입력, 리그 생성)
	
    // 게임 메인 루프: 종료 조건을 만날 때까지 반복
    while (true) {
        showMainMenu();  // 현재 상태와 명령어 목록 출력

        // 명령어 입력받기
        std::cout << "명령어 입력: ";
        std::string cmd;
        if (!std::getline(std::cin, cmd)) break;  // 입력 스트림이 닫히면(EOF) 루프 종료

        // 영어 명령어 비교를 위해 소문자로 변환한 버전도 준비
        // 한글 명령어는 cmd 원본으로 비교, 영어 명령어는 cmdLower로 비교
        std::string cmdLower = toLower(cmd);

        // 입력된 명령어에 따라 해당 기능 실행
        if (cmd == "일정" || cmdLower == "schedule") {
            // 리그가 이미 끝났으면 안내 메시지만 출력
            if (league.isFinished()) {
                std::cout << "모든 리그 경기가 종료되었습니다.\n";
            } else {
                doSchedule();  // 다음 경기 진행
            }
        } else if (cmd == "로스터" || cmdLower == "roster") {
            doRoster();        // 선수 명단/포메이션 확인 및 변경
        } else if (cmd == "트레이드" || cmdLower == "trade") {
            doTrade();         // 선수 트레이드
        } else if (cmd == "순위" || cmdLower == "standing") {
            league.printStandings();  // 현재 순위표 출력
            waitEnter();
        } else if (cmd == "종료" || cmdLower == "exit" || cmdLower == "quit") {
            std::cout << "\n게임을 종료합니다. 진행 상황은 저장되지 않습니다.\n";
            break;             // while 루프 탈출 -> 게임 종료
        } else {
            // 위의 어느 명령어도 해당하지 않으면
            std::cout << "알 수 없는 명령어입니다.\n";
        }

        // 명령어 처리 후 리그가 끝났는지 확인 (마지막 경기 진행 직후 true가 됨)
        if (league.isFinished()) {
            std::cout << "\n리그전이 모두 종료되었습니다!\n";
            league.printStandings();  // 최종 리그 순위표 출력
            waitEnter();

            std::cout << "플레이오프를 시작합니다.\n";
            waitEnter();

            // 플레이오프 진행 (우승 팀 이름 반환)
            std::string champion = runPlayoff(league);

            waitEnter();

            // 새 시즌 여부 결정
            std::cout << "\n새 시즌을 시작하시겠습니까?\n";
            std::cout << "(y) 현재 팀으로 새 시즌  (n) 게임 종료\n입력: ";
            std::string ans;
            std::getline(std::cin, ans);

            if (ans == "y" || ans == "Y") {
                // 새 시즌: 선수 명단/포메이션/전술은 유지하고 성적만 초기화
				tradeCount = 0;
                // 현재 플레이어 팀 데이터 백업
                std::vector<Player> savedRoster     = league.teams[0].roster;         // 선수 명단
                int                 savedFormation  = league.teams[0].formationIndex;  // 포메이션
                Tactic              savedTactic      = league.teams[0].tactic;          // 전술

                // 리그 전체 초기화 (AI 팀 새로 생성, 일정 새로 생성)
                league.init(playerTeamName);

                // 백업해둔 플레이어 팀 데이터 복원
                league.teams[0].roster         = savedRoster;      // 선수 명단 복원
                league.teams[0].formationIndex = savedFormation;   // 포메이션 복원
                league.teams[0].tactic         = savedTactic;       // 전술 복원
                league.teams[0].applyFormation();                  // 복원된 포메이션으로 역할 재배정

                hasPlayedMatch = false;  // 새 시즌이므로 첫 경기 여부 초기화

                std::cout << "\n새 시즌이 시작되었습니다!\n";
                waitEnter();
            } else {
                // 새 시즌을 선택하지 않으면 게임 종료
                std::cout << "\n게임을 종료합니다.\n";
                break;  // while 루프 탈출
            }
        }
    }
}


// =============================================================
// showMainMenu()
// 메인 메뉴를 출력한다.
// 현재 팀 상태(경기 수, 순위, 승점, 다음 상대)와 명령어 목록을 보여준다.
// =============================================================
void Game::showMainMenu() {
    printLine();

    // 현재 리그 진행 상황 요약
    int played = league.playedMatches();  // 지금까지 진행한 경기 수
    std::cout << "【" << playerTeamName << "】"
              << "  리그 " << played << "/" << league.totalMatches() << "경기"  // 예: "리그 5/38경기"
              << "  |  순위: " << league.playerRank() << "위"                   // 예: "순위: 3위"
              << "  |  승점: " << league.teams[0].displayPoints() << "\n";      // 예: "승점: 12"

    // 리그가 아직 진행 중이면 다음 상대 표시
    if (!league.isFinished()) {
        std::string opp = league.nextOpponentName();  // 다음 경기 상대 이름
        if (!opp.empty())
            std::cout << "다음 상대: " << opp << "\n";  // 예: "다음 상대: Red Dragons FC"
    }

    printLine();

    // 사용 가능한 명령어 목록 출력
    std::cout << "  일정     - 다음 경기 진행\n";
    std::cout << "  로스터   - 선수 확인 및 포메이션 변경\n";
    std::cout << "  트레이드 - 선수 트레이드\n";
    std::cout << "  순위     - 현재 순위표\n";
    std::cout << "  종료     - 게임 종료\n";
    printLine();
}


// =============================================================
// chooseTactic()
// 3가지 전술을 출력하고 플레이어가 번호를 입력해 선택하게 한다.
// 반환된 Tactic 값은 doSchedule()에서 playPlayerMatch()로 전달된다.
// =============================================================
Tactic Game::chooseTactic() {
    printLine();
    std::cout << "전술을 선택하세요:\n";
    std::cout << "  1. 공격적 전술\n";   // 기획서 6.2.2
    std::cout << "  2. 균형적 전술\n";
    std::cout << "  3. 수비적 전술\n";
    std::cout << "입력 (1~3): ";

    int t = -1;  // 유효하지 않은 초기값

    // 1~3 범위의 유효한 입력이 들어올 때까지 반복
    while (t < 1 || t > 3) {
        std::string s;
        std::getline(std::cin, s);                        // 한 줄 입력
        try { t = std::stoi(s); } catch (...) { t = -1; } // 정수 변환 시도, 실패하면 -1
        if (t < 1 || t > 3)
            std::cout << "1~3 중에 입력하세요: ";
    }

    // 입력 번호를 Tactic 열거형으로 변환해서 반환
    switch (t) {
        case 1: return Tactic::ATTACK;   // 1 -> 공격 전술
        case 2: return Tactic::BALANCE;  // 2 -> 균형 전술
        default: return Tactic::DEFENSE; // 3 -> 수비 전술 (default로 처리)
    }
}


// =============================================================
// doSchedule()
// "일정" 명령어 처리. 다음 경기를 진행한다.
// =============================================================
void Game::doSchedule() {
    // 다음 상대 이름 확인 (없으면 경기가 없는 상태)
    std::string opp = league.nextOpponentName();
    if (opp.empty()) {
        std::cout << "더 이상 진행할 경기가 없습니다.\n";
        return;  // 함수 종료 (경기 없음)
    }

    printLine();
    std::cout << "다음 상대: " << opp << "\n";
    // 현재 포메이션 표시 (경기 전 마지막 확인)
    std::cout << "현재 포메이션: " << FORMATIONS[league.teams[0].formationIndex].label << "\n";

    // 전술 선택
    Tactic tactic = chooseTactic();

    std::cout << "\n[" << playerTeamName << "] "
              << tacticToString(tactic) << " 선택!\n";
    waitEnter();  // 경기 시작 전 잠시 대기

    // 실제 경기 진행 (결과는 플레이어 기준 WIN/DRAW/LOSS)
    lastMatchResult = league.playPlayerMatch(tactic);
    hasPlayedMatch  = true;  // 첫 경기 완료 표시 (이제 트레이드 가능)

    // 경기 결과 출력
    std::cout << "\n결과: ";
    if (lastMatchResult == MatchResult::WIN)       std::cout << "승리!\n";
    else if (lastMatchResult == MatchResult::DRAW) std::cout << "무승부\n";
    else                                           std::cout << "패배\n";

	if (lastMatchResult == MatchResult::LOSS) {
		tradeCount += 2;}
	else { tradeCount += 1; }

    waitEnter();
    league.printStandings();  // 경기 후 업데이트된 순위표 출력
    waitEnter();
}


// =============================================================
// doRoster()
// "로스터" 명령어 처리. 선수 명단을 출력하고 포메이션 변경 여부를 묻는다.
// =============================================================
void Game::doRoster() {
    league.teams[0].printRoster();  // 플레이어 팀(teams[0])의 로스터 출력

    // 포메이션 변경 여부 확인
    std::cout << "\n포메이션을 변경하시겠습니까? (y/n): ";
    std::string ans;
    std::getline(std::cin, ans);

    if (ans == "y" || ans == "Y")
        changeFormationMenu();  // 포메이션 변경 서브메뉴 진입
    else
        waitEnter();  // 변경하지 않으면 메인 메뉴로 돌아감
}


// =============================================================
// changeFormationMenu()
// 포메이션 변경 서브메뉴. 10가지 포메이션을 보여주고 선택받는다.
// =============================================================
void Game::changeFormationMenu() {
    printLine();
    std::cout << "포메이션 선택:\n";

    // FORMATION_COUNT (=10)만큼 반복하며 포메이션 목록 출력
    for (int i = 0; i < (int)FORMATION_COUNT; ++i)
        std::cout << (i + 1) << ". " << FORMATIONS[i].label << "\n";  // 예: "1. 3-5-2"

    std::cout << "번호 입력 (1~10): ";

    int pick = -1;  // 유효하지 않은 초기값

    // 1~10 범위의 유효한 입력이 들어올 때까지 반복
    while (pick < 1 || pick > 10) {
        std::string s;
        std::getline(std::cin, s);
        try { pick = std::stoi(s); } catch (...) { pick = -1; }
        if (pick < 1 || pick > 10)
            std::cout << "1~10 중에 입력하세요: ";
    }

    // 선택한 번호로 포메이션 변경 (배열 인덱스는 0부터이므로 pick-1)
    league.teams[0].changeFormation(pick - 1);

    std::cout << "포메이션 변경 완료: " << FORMATIONS[pick - 1].label << "\n";
    league.teams[0].printRoster();  // 변경된 포메이션으로 로스터 다시 출력
    waitEnter();
}


// =============================================================
// doTrade()
// "트레이드" 명령어 처리.
// 첫 경기 전이면 트레이드 불가 안내, 이후엔 runTrades() 호출.
// =============================================================
void Game::doTrade() {
    if (!hasPlayedMatch) {
        // 아직 한 경기도 진행하지 않은 상태
        std::cout << "트레이드는 첫 경기 이후부터 가능합니다.\n";
        waitEnter();
        return;  // 함수 종료
    }
	else if (tradeCount == 0) {
		std::cout << "남은 트레이드 횟수가 없습니다.\n";
		waitEnter();
		return;
	}
	else
    tradeCount = runTrades(league.teams[0], tradeCount);
    waitEnter();
}
