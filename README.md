# TheBattleFront

<img src ="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white"> <img src ="https://img.shields.io/badge/Unreal Engine-0E1128?style=for-the-badge&logo=unreal engine&logoColor=white"> <img src ="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white">

* 개발 기간 : 2024.03 - 2024.06
* 팀원 : 1 서버 프로그래머 + 2 클라이언트 프로그래머
* 담당 : 캐릭터, UI


## Game Overview

이 게임은 **언리얼 엔진**을 활용하여 제작된 **슈팅 기반 배틀로얄 멀티플레이 게임**입니다. 로그라이크 요소가 추가되어, 플레이어들은 몬스터를 처치하고 레벨을 올리며 스킬을 선택할 수 있습니다. 최종 목표는 전투에서 마지막까지 살아남아 승리하는 것입니다.

## Features

- **멀티플레이 환경 구축**: 서버를 활용하여 안정적인 멀티플레이 환경을 구축.
  
- **UI 관리**: Widget(UI)을 오브젝트화하여 MainWidget에서 일괄적으로 관리.
  
- **데이터 관리**: CSV 파일을 활용하여 캐릭터와 몬스터의 데이터를 효율적으로 관리.
  
- **시야 차단 구역**: 특정 구역에 들어가면 상대방의 시야에서 사라지는 시스템 구현.
  
- **다양한 스킬 구현**: 미사일, 드론, 체력 회복, 독 구름, 방어막, 수류탄 스킬을 구현하여 다양한 스킬 조합 가능.
  
- **맵 아이템 배치**: 전투에 필요한 아이템을 맵에 전략적으로 배치(체력 회복, 공격력 증가, 방어력 증가, 레벨 업 등).
  
- **자기장 시스템**: 게임 진행에 따라 점차 좁혀지는 안전 구역(자기장) 시스템 구현.
  
- **지도 및 미니맵**: 전체 맵의 지도와 실시간 미니맵을 구현하여 플레이어의 위치 파악을 지원.

## How To Play
* 이동 : 키보드 W, A, S, D
* 공격 : 마우스 좌 클릭

|  |   |
|---|---|
|시작 화면|접속 완료|
|<img width="450" alt="image" src="https://github.com/user-attachments/assets/a1803763-faaa-422c-94bd-88fde25b1ac9">|<img width="450" alt="image" src="https://github.com/user-attachments/assets/b3331a0f-41f4-455d-ba16-b96a9e1f475c">|
|인게임 화면1|인게임 화면2|
|<img width="450" alt="image" src="https://github.com/user-attachments/assets/1a63e357-bac7-46cc-ac1a-f75e14725d67">|<img width="450" alt="image" src="https://github.com/user-attachments/assets/3d617a05-a591-44e2-a82e-8690e646cd2a">|
|맵과 미니맵| 결과 화면 |
|<img width="450" alt="image" src="https://github.com/user-attachments/assets/d4a66f3a-0e22-46ac-b13c-5c928375a21a"> |<img width="450" alt="image" src="https://github.com/user-attachments/assets/e5efa3e2-f82e-47bd-bea2-aeb36137c638">
