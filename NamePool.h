// =============================================================
// NamePool.h
// 선수 이름과 팀 이름을 무작위로 뽑기 위한 이름 풀(pool) 데이터 파일.
//
// 이 파일은 구현 코드 없이 데이터만 가지고 있다.
// Player.cpp에서 generateRandomName() 을 호출할 때
// 이 풀에서 성(LAST_NAMES)과 이름(FIRST_NAMES)을 하나씩 랜덤으로 골라 조합한다.
//
// 선수 이름 생성 예시:
//   FIRST_NAMES[랜덤] + " " + LAST_NAMES[랜덤]
//   예: "Marcus" + " " + "Silva" = "Marcus Silva"
// =============================================================

#pragma once          // 이 헤더가 여러 파일에서 include 되어도 한 번만 처리되도록 보장
#include <vector>     // std::vector (동적 배열) 타입 사용
#include <string>     // std::string 타입 사용


// -------------------------------------------------------------
// FIRST_NAMES
// 선수 이름(성 제외)의 풀. 총 50개.
// 다양한 국적의 이름을 섞어 국제적인 리그 분위기를 만든다.
// -------------------------------------------------------------
const std::vector<std::string> FIRST_NAMES = {
    // 남미 계열
    "Marcus", "Joao", "Luca", "Carlos", "Diego",
    "Andre", "Felipe", "Bruno", "Rafael", "Gabriel",
    // 영어권
    "James", "Oliver", "Harry", "Ethan", "Noah",
    "Lucas", "Mason", "Logan", "Elijah", "Aiden",
    // 유럽
    "Hugo", "Theo", "Leon", "Finn", "Oscar",
    // 동유럽
    "Ivan", "Alexei", "Dmitri", "Sergei", "Pavel",
    // 중동
    "Ahmed", "Omar", "Youssef", "Karim", "Hassan",
    // 아시아
    "Hiroshi", "Kenji", "Takashi", "Ryo", "Daiki",
    // 중남미
    "Santiago", "Mateo", "Sebastian", "Nicolas", "Alejandro",
    // 아프리카
    "Kwame", "Kofi", "Amos", "Emeka", "Chidi"
};


// -------------------------------------------------------------
// LAST_NAMES
// 선수 성(姓)의 풀. 총 50개.
// FIRST_NAMES와 조합하면 최대 2500가지 이름이 나올 수 있다.
// -------------------------------------------------------------
const std::vector<std::string> LAST_NAMES = {
    // 포르투갈/브라질 계열
    "Silva", "Santos", "Oliveira", "Costa", "Ferreira",
    // 영어권
    "Smith", "Johnson", "Williams", "Brown", "Jones",
    // 독일 계열
    "Muller", "Schmidt", "Fischer", "Weber", "Meyer",
    // 스페인/중남미 계열
    "Garcia", "Martinez", "Lopez", "Hernandez", "Gonzalez",
    // 한국 계열
    "Kim", "Park", "Lee", "Choi", "Jung",
    // 러시아 계열
    "Ivanov", "Petrov", "Sidorov", "Kozlov", "Volkov",
    // 일본 계열
    "Nakamura", "Tanaka", "Suzuki", "Ito", "Yamamoto",
    // 서아프리카 계열
    "Diallo", "Traore", "Coulibaly", "Toure", "Camara",
    // 중동/아프리카 계열
    "Al-Rashid", "Hassan", "Ibrahim", "Okafor", "Mensah",
    // 이탈리아 계열
    "Rossi", "Ferrari", "Esposito", "Romano", "Bianchi"
};


// -------------------------------------------------------------
// TEAM_NAMES
// AI 팀 19개의 이름 풀.
// League::init() 에서 AI 팀을 생성할 때 순서대로 사용한다.
// (TEAM_NAMES[0] = 1번째 AI팀, TEAM_NAMES[18] = 19번째 AI팀)
// -------------------------------------------------------------
const std::vector<std::string> TEAM_NAMES = {
    "Red Dragons FC",
    "Blue Wolves United",
    "Golden Eagles SC",
    "Iron Lions FC",
    "Storm Petrels FC",
    "Silver Foxes United",
    "Black Panthers SC",
    "White Tigers FC",
    "Crimson Hawks United",
    "Ocean Sharks FC",
    "Desert Falcons SC",
    "Mountain Bears United",
    "Thunder Bulls FC",
    "Frost Giants SC",
    "Ember Phoenix United",
    "Shadow Wolves FC",
    "Steel Ravens SC",
    "Wild Boars United",
    "Neon Jaguars FC"   // 19번째 AI팀
};
