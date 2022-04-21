#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	//3Dモデルの生成
	model_ = Model::Create();

	// X,Y,Z 方向のスケーリングを設定
	for (int i = 0; i < _countof(worldTransform_); i++) {

		if (i < 11) 
		{
			worldTransform_[i].scale_ = {5.0f, 5.0f, 5.0f};

			// X,Y,Z 軸回りの回転角を設定
			worldTransform_[i].rotation_ = {0.0f, 0.0f, 0.0f};

			// X,Y,Z 軸回りの平行移動を設定
			worldTransform_[i].translation_ = {-54.5f + (i * 10.0f), 23.5f, 8.5f};
		} 

		else if (i >= 11)
		{
			// if (worldTransform_[i].translation_.x <= )
			worldTransform_[i].scale_ = {5.0f, 5.0f, 5.0f};

			// X,Y,Z 軸回りの回転角を設定
			worldTransform_[i].rotation_ = {0.0f, 0.0f, 0.0f};

			// X,Y,Z 軸回りの平行移動を設定
			worldTransform_[i].translation_ = {-54.5f + ((i - 11) * 10.0f), -22.5f, 8.5f};
		}
		
		//ワールドトランスフォームの初期化
		worldTransform_[i].Initialize();
	}
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

}

void GameScene::Update() {}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	
	// 3Dモデル描画
	for (int i = 0; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}

	/// </summary>
	 
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
