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

	// 3Dモデルの生成
	model_ = Model::Create();

	////乱数シード生成器
	//std::random_device seed_gen;

	////メルセンヌ・ツイスター
	//std::mt19937_64 engine(seed_gen());

	////乱数範囲(回転角用)
	//std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	////乱数範囲(座標用)
	//std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	// X,Y,Z 方向のスケーリングを設定

	//親
	// X,Y,Z 方向のスケーリングを設定
	worldTransform_[0].scale_ = {1.0f, 1.0f, 1.0f};
	// X,Y,Z 軸回りの回転角を設定
	worldTransform_[0].rotation_ = {0.0f, 0.0f, 0.0f};
	// X,Y,Z 軸回りの平行移動を設定
	worldTransform_[0].translation_ = {0.0f, 0.0f, 3.0f};
	
	for (int i = 1; i < 5; i++) 
	{
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
		// X,Y,Z 軸回りの回転角を設定
		worldTransform_[i].rotation_ = {0.0f, 0.0f, 0.0f};
		// X,Y,Z 軸回りの平行移動を設定
		worldTransform_[i].translation_ = {0.0f, 0.0f, 3.0f};
		
		worldTransform_[i].Initialize();
	}

	worldTransform_[5].scale_ = {1.0f, 1.0f, 1.0f};
	// X,Y,Z 軸回りの回転角を設定
	worldTransform_[5].rotation_ = {0.0f, 0.0f, 0.0f};
	// X,Y,Z 軸回りの平行移動を設定
	worldTransform_[5].translation_ = {10.0f, 0.0f, 3.0f};
	
	worldTransform_[5].Initialize();

	worldTransform_[PartID::Center].Initialize();

	worldTransform_[PartID::Right].translation_ = {3.0f, 0.0f, 0.0f};
	worldTransform_[PartID::Right].parent_ = &worldTransform_[PartID::Center];
	worldTransform_[PartID::Right].Initialize();

	worldTransform_[PartID::Left].translation_ = {-3.0f, 0.0f, 0.0f};
	worldTransform_[PartID::Left].parent_ = &worldTransform_[PartID::Center];
	worldTransform_[PartID::Left].Initialize();

	worldTransform_[PartID::Up].translation_ = {0.0f, 3.0f, 0.0f};
	worldTransform_[PartID::Up].parent_ = &worldTransform_[PartID::Center];
	worldTransform_[PartID::Up].Initialize();

	worldTransform_[PartID::Down].translation_ = {0.0f, -3.0f, 0.0f};
	worldTransform_[PartID::Down].parent_ = &worldTransform_[PartID::Center];
	worldTransform_[PartID::Down].Initialize();

	//カメラ視点座標を設定
	//viewProjection_.eye = {0.0f, 0.0f, 0.0f};

	//カメラ注視点座標を設定
	//viewProjection_.target = {0.0f, 0.0f, 0.0f};
	//viewProjection_.target = {worldTransform_[0].rotation_.x, 0.0f, 0.0f};

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

	//回転移動の速さ
	const float rotateSpeed = 0.1f;

	if (input_->PushKey(DIK_LEFT))	//左キーを押して左回転
	{
		worldTransform_[0].rotation_.y += 0.1f;
	}
	else if (input_->PushKey(DIK_RIGHT)) //右キーを押して右回転
	{
		worldTransform_[0].rotation_.y -= 0.1f;
	}

	//回転移動の単位ベクトル
	XMFLOAT3 frontVec = {0, 0, 1};

	//演算ベクトル
	XMFLOAT3 resultVec = {0, 0, 0};

	//注視点ベクトルをどうするかで決まる。
	
	//回転移動後の座標計算
	resultVec.x = cosf(worldTransform_[PartID::Center].rotation_.y) * frontVec.x +
	              sinf(worldTransform_[PartID::Center].rotation_.y) * frontVec.z;
	resultVec.z = -sinf(worldTransform_[PartID::Center].rotation_.y) * frontVec.x +
	              cosf(worldTransform_[PartID::Center].rotation_.y) * frontVec.z;

	//キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_W)) {
		worldTransform_[0].translation_.x += (resultVec.x * kCharacterSpeed);
		worldTransform_[0].translation_.z += (resultVec.z * kCharacterSpeed);
	}

	if (input_->PushKey(DIK_S)) {
		worldTransform_[0].translation_.x -= (resultVec.x * kCharacterSpeed);
		worldTransform_[0].translation_.z -= (resultVec.z * kCharacterSpeed);
	}

	//親の行列の再計算
	worldTransform_[0].UpdateMatrix();

	//子の行列の再計算
	for (int i = 1; i < _countof(worldTransform_); i++)
	{
		worldTransform_[i].UpdateMatrix();
	}

	//カメラベクトルの作成
	XMFLOAT3 cameraVec = {resultVec.x, 0, resultVec.z};

	//オブジェクトとの距離
	float cameraDistance = 30.0f;

	//カメラ座標の計算
	viewProjection_.eye.x = worldTransform_[0].translation_.x + cameraDistance * cameraVec.x;
	viewProjection_.eye.y = worldTransform_[0].translation_.y + cameraDistance * cameraVec.y;
	viewProjection_.eye.z = worldTransform_[0].translation_.z + cameraDistance * cameraVec.z;

	//注視点座標の設定
	viewProjection_.target = worldTransform_[0].translation_;

	//行列の再計算
	viewProjection_.UpdateMatrix();
}

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
	/// </summary>

	// 3Dモデル描画
	for (int i = 0; i < _countof(worldTransform_); i++) {

		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);

	//視点移動のデバッグ用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	//注視点移動のデバッグ用表示
	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
	  viewProjection_.target.z);

	//連続回転デバッグ用表示
	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);

	/*debugText_->SetPos(50, 70);
	debugText_->Printf("translation:(%f,%f,%f)", 10.0f, 10.0f, 10.0f);
	debugText_->SetPos(50, 90);
	debugText_->Printf("rotation:(%f,%f,%f)", XM_PI / 4.0f, XM_PI / 4.0f, 0.0f);
	debugText_->SetPos(50, 110);
	debugText_->Printf("translation:(%f,%f,%f)", 5.0f, 5.0f, 5.0f);*/

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
