# TheBattleFront

<img src ="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white"> <img src ="https://img.shields.io/badge/Unreal Engine-0E1128?style=for-the-badge&logo=unreal engine&logoColor=white"> <img src ="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white">

* 開発期間 : 2024.03 - 2024.06
* チームメンバー :  サーバープログラマー１名 + クライアントプログラマー２名
* 担当 : [キャラクター](https://github.com/HongSongUi/TheBattleFront/tree/main/TheBattleFront/Source/TheBattleFront/Character), [スキル](https://github.com/HongSongUi/TheBattleFront/tree/main/TheBattleFront/Source/TheBattleFront/Ability), [UI](https://github.com/HongSongUi/TheBattleFront/tree/main/TheBattleFront/Source/TheBattleFront/Widget), [武器](https://github.com/HongSongUi/TheBattleFront/tree/main/TheBattleFront/Source/TheBattleFront/Weapon)


## Game Overview

このゲームは**アンリアルエンジン**を活用して製作された**シューティングベースのバトルロイヤルマルチプレイゲーム**です。 ローグライク要素が追加され、プレイヤーはモンスターを処置し、レベルを上げることでスキルを選択できます。 最終目標は戦闘で最後まで生き残って勝利することです。

## Features

- **マルチプレイ環境**:サーバーを活用して安定したマルチプレイ環境を構築。

- **UI管理**:Widget(UI)をオブジェクト化してMainWidgetで一括的に管理。

- **データ管理**:CSVファイルを活用してキャラクターとモンスターのデータを効率的に管理。

- **視野遮断区域**:特定区域に入ると相手の視野から消えるシステムを実現。

- **様々なスキル実装**: ミサイル、ドローン、体力回復、毒霧、防御膜、手榴弾スキルを実装して様々なスキルを組み合わせることが可能。

- **マップアイテム配置**:戦闘に必要なアイテムをマップに戦略的に配置（体力回復、攻撃力増加、防御力増加、レベルアップなど）。

- **磁場システム**:ゲームの進行に応じて徐々に狭まる安全区域(磁場)システムを実現。

- **地図及びミニマップ**:全体マップの地図とリアルタイムミニマップを具現化し、プレイヤーの位置把握を支援。

## How To Play
* 移動:キーボード W·A·S·D
* 攻撃:マウス 左 クリック


|  |   |
|---|---|
|スタート画面|Host画面|
|<img width="450" alt="image" src="https://github.com/user-attachments/assets/f517b343-d445-4c89-96d0-0aa07c4566e2">|<img width="450" alt="image" src="https://github.com/user-attachments/assets/3c3103c9-26ae-4799-941c-2b705e3175e2">|
|Client画面|接続の画面|
|<img width="450" alt="image" src="https://github.com/user-attachments/assets/a1803763-faaa-422c-94bd-88fde25b1ac9">|<img width="450" alt="image" src="https://github.com/user-attachments/assets/b3331a0f-41f4-455d-ba16-b96a9e1f475c">|
|ゲームの画面1|ゲームの画面2|
|<img width="450" alt="image" src="https://github.com/user-attachments/assets/1a63e357-bac7-46cc-ac1a-f75e14725d67">|<img width="450" alt="image" src="https://github.com/user-attachments/assets/3d617a05-a591-44e2-a82e-8690e646cd2a">|
|マップとミニマップ| 結果画面 |
|<img width="450" alt="image" src="https://github.com/user-attachments/assets/d4a66f3a-0e22-46ac-b13c-5c928375a21a"> |<img width="450" alt="image" src="https://github.com/user-attachments/assets/e5efa3e2-f82e-47bd-bea2-aeb36137c638">
