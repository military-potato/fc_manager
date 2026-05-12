#include "Trade.h"   // calcTradeCount(), doTrade(), runTrades() 선언
#include "Utils.h"   // printLine()
#include <iostream>  // std::cout, std::cin
#include <string>    // std::string, std::stoi()
#include <algorithm> // (현재 직접 사용하지 않음)


// =============================================================
// calcTradeCount()
// 경기 결과에 따라 트레이드 가능 횟수를 반환한다.
// 기획서 6.4: 패배 시 2회, 그 외(승리/무승부) 1회
// =============================================================


void doTrade(Team& playerTeam) {
    printLine();
    std::cout << "【선수 트레이드】\n\n";
    
    // 선택한 번호에 해당하는 후보 선수 저장
    // pick은 1~3이고 배열 인덱스는 0~2이므로 pick-1로 접근
    Player newPlayer = generateRandomPlayer();

    // 방출할 선수 선택을 위해 현재 로스터 출력
    printLine();
    std::cout << "현재 로스터:\n";
    for (int i = 0; i < (int)playerTeam.roster.size(); ++i) {
        std::cout << (i + 1) << ". ";        // 1~10 번호 출력
        playerTeam.roster[i].printInfo();     // 선수 정보 출력
    }

    // 방출할 선수 번호 입력받기
    std::cout << "\n방출할 선수 번호를 입력하세요 (1~10): ";
    int release = -1;

    // release가 1~10 범위 안에 들어올 때까지 반복
    while (release < 1 || release > 10) {
        std::string input;
        std::getline(std::cin, input);
        try { release = std::stoi(input); } catch (...) { release = -1; }
        if (release < 1 || release > 10)
            std::cout << "1~10 사이의 숫자를 입력하세요: ";
    }

    // 방출할 선수 이름 저장 (방출 확인 메시지 출력용)
    Player releasedPlayer = playerTeam.roster[release - 1];

    // roster에서 방출 선수 자리에 새 선수를 덮어쓴다
    // release는 1~10이고 배열 인덱스는 0~9이므로 release-1로 접근
    playerTeam.roster[release - 1] = newPlayer;

    // 선수가 바뀌었으므로 포메이션에 맞게 역할을 다시 배정
    // (새 선수의 role이 아직 MIDFIELDER로 초기화되어 있으므로 재배정 필요)
    playerTeam.applyFormation();

    // 트레이드 완료 메시지 출력
    std::cout << "\n";
    releasedPlayer.printInfo();
    std::cout<< "방출\n";
    newPlayer.printInfo();
    std::cout << "영입 완료!\n";
    printLine();
}


// =============================================================
// runTrades()
// 트레이드 가능 횟수만큼 doTrade()를 반복 호출한다.
// 각 트레이드마다 진행 여부를 물어보고 "n" 입력 시 건너뛸 수 있다.
// =============================================================
int runTrades(Team& playerTeam, int count) {
    // 경기 결과에 따라 트레이드 횟수 계산 
    std::cout << "\n트레이드 가능 횟수: " << count << "회\n";


    std::cout << "트레이드를 진행하시겠습니까?\n(주의! 방츨한 선수는 되돌릴 수 없으며 영입한 선수의 능력치는 랜덤으로 정해집니다.)\n(y/n): ";
    std::string ans;
    std::getline(std::cin, ans);  // "y" 또는 "n" 입력받기
    
    if (ans == "y" || ans == "Y")
    {
		
        doTrade(playerTeam);
		return count-1;
    }
	return count;
}

