#include <Novice.h>
#include <cmath>
#include <cassert>

const char kWindowTitle[] = "LE2B_12_サクライショウセイ_タイトル";

struct Vector3 {
	float x, y, z;
};

struct Matrix4x4 {
	float m[4][4];
};

struct Quaternion{
	float x;
	float y;
	float z;
	float w;
};

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix) {
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight, "%6.03f", matrix.m[row][column]);
		}
	}
}

void QuaternionScreenPrintf(int x, int y, const Quaternion& quaternion, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", quaternion.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", quaternion.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", quaternion.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%.02f", quaternion.w);
	Novice::ScreenPrintf(x + kColumnWidth * 4, y, "%s", label);
}
Vector3 Add(const Vector3& v1, const Vector3& v2) { return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); };
Vector3 Multiply(float scalar, const Vector3& v) { return Vector3(scalar * v.x, scalar * v.y, scalar * v.z); };
float Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
float Length(const Vector3& v) { return std::sqrt(Dot(v, v)); }
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}

Vector3 Normalize(const Vector3& v) {
	float length = Length(v);
	assert(length != 0.0f);
	return { v.x / length, v.y / length, v.z / length };
}

Vector3 operator+ (const Vector3 v1, const Vector3 v2) {
	return Add(v1, v2);
}

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);
Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

// Quaternionの積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
// 単位Quaternionを返す
Quaternion IdentityQuaternion();
// 共役Quaternionを返す
Quaternion Conjugate(const Quaternion& quaternion);
// Quaternionのnormを返す
float Norm(const Quaternion& quaternion);
// 正規化したQuaternionを返す
Quaternion Normalize(const Quaternion& quaternion);
// 逆Quaternionを返す
Quaternion Inverse(const Quaternion& quaternion);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};
	Quaternion q1 = { 2.0f, 3.0f, 4.0f, 1.0f };
	Quaternion q2 = { 1.0f, 3.0f, 5.0f, 2.0f };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Quaternion identity = IdentityQuaternion();
		Quaternion conj = Conjugate(q1);
		Quaternion inv = Inverse(q1);
		Quaternion normal = Normalize(q1);
		Quaternion mul1 = Multiply(q1, q2);
		Quaternion mul2 = Multiply(q2, q1);
		float norm = Norm(q1);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		QuaternionScreenPrintf(0, 0, identity, ": Identity");
		QuaternionScreenPrintf(0, kRowHeight, conj, ": Conjugate");
		QuaternionScreenPrintf(0, kRowHeight * 2, inv, ": Inverse");
		QuaternionScreenPrintf(0, kRowHeight * 3, normal, ": Normalize");
		QuaternionScreenPrintf(0, kRowHeight * 4, mul1, ": Multiply(q1, q2)");
		QuaternionScreenPrintf(0, kRowHeight * 5, mul2, ": Multiply(q2, q1)");
		Novice::ScreenPrintf(0, kRowHeight * 6, "%.02f  : Norm", norm);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle)
{
	Matrix4x4 R;

	R.m[0][0] = axis.x * axis.x * (1 - cosf(angle)) + cosf(angle);
	R.m[0][1] = axis.x * axis.y * (1 - cosf(angle)) + axis.z * sinf(angle);
	R.m[0][2] = axis.x * axis.z * (1 - cosf(angle)) - axis.y * sinf(angle);
	R.m[0][3] = 0;

	R.m[1][0] = axis.x * axis.y * (1 - cosf(angle)) - axis.z * sinf(angle);
	R.m[1][1] = axis.y * axis.y * (1 - cosf(angle)) + cosf(angle);
	R.m[1][2] = axis.y * axis.z * (1 - cosf(angle)) + axis.x * sinf(angle);
	R.m[1][3] = 0;

	R.m[2][0] = axis.x * axis.z * (1 - cosf(angle)) + axis.y * sinf(angle);
	R.m[2][1] = axis.y * axis.z * (1 - cosf(angle)) - axis.x * sinf(angle);
	R.m[2][2] = axis.z * axis.z * (1 - cosf(angle)) + cosf(angle);
	R.m[2][3] = 0;

	R.m[3][0] = 0;
	R.m[3][1] = 0;
	R.m[3][2] = 0;
	R.m[3][3] = 1;
	
	return R;
}

Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to)
{
	Vector3 cross = Cross(from, to);
	float cos = Dot(from, to);
	float sin = Length(from) * Length(to);
	float epsilon = 1e-6f;
	Vector3 axis = {};
	if (std::abs(cos + 1.0f) <= epsilon) {
		if (std::abs(from.x) > epsilon || std::abs(from.y) > epsilon) {
			//(ux≠0||uy≠0)の際のaxisの値を入れる
			axis.x = from.y;
			axis.y = -from.x;
			axis.z = 0;
		} else if (std::abs(from.x) > epsilon || std::abs(from.z) > epsilon) {
			//(ux≠0||uz≠0)の際のaxisの値を入れる
			axis.x = from.z;
			axis.y = 0;
			axis.z = -from.x;
		} else {
			// zero vector
			assert(false);
		}

		axis = Normalize(axis);
	} else {
		axis = Normalize(cross);
	}

	Matrix4x4 R;

	R.m[0][0] = axis.x * axis.x * (1 - cos) + cos;
	R.m[0][1] = axis.x * axis.y * (1 - cos) + axis.z * sin;
	R.m[0][2] = axis.x * axis.z * (1 - cos) - axis.y * sin;
	R.m[0][3] = 0;

	R.m[1][0] = axis.x * axis.y * (1 - cos) - axis.z * sin;
	R.m[1][1] = axis.y * axis.y * (1 - cos) + cos;
	R.m[1][2] = axis.y * axis.z * (1 - cos) + axis.x * sin;
	R.m[1][3] = 0;

	R.m[2][0] = axis.x * axis.z * (1 - cos) + axis.y * sin;
	R.m[2][1] = axis.y * axis.z * (1 - cos) - axis.x * sin;
	R.m[2][2] = axis.z * axis.z * (1 - cos) + cos;
	R.m[2][3] = 0;

	R.m[3][0] = 0;
	R.m[3][1] = 0;
	R.m[3][2] = 0;
	R.m[3][3] = 1;

	return R;
}

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	Quaternion qr;
	Vector3 lv = { lhs.x, lhs.y, lhs.z };
	Vector3 rv = { rhs.x, rhs.y, rhs.z };
	Vector3 qrv = Cross(lv, rv) + Multiply(rhs.w, lv) + Multiply(lhs.w, rv);
	qr = Quaternion(qrv.x, qrv.y, qrv.z, (lhs.w * rhs.w) - Dot(lv, rv));

	return qr;
}

Quaternion IdentityQuaternion()
{
	return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion Conjugate(const Quaternion& quaternion)
{
	Vector3 a;

	a = Vector3(-quaternion.x, -quaternion.y, -quaternion.z);

	return Quaternion(a.x, a.y, a.z, quaternion.w);
}

float Norm(const Quaternion& quaternion)
{
	float a = sqrtf((quaternion.w * quaternion.w) + (quaternion.x * quaternion.x) + (quaternion.y * quaternion.y) + (quaternion.z * quaternion.z));

	return a;
}

Quaternion Normalize(const Quaternion& quaternion)
{
	float a = Norm(quaternion);

	return Quaternion(quaternion.x / a, quaternion.y / a, quaternion.z / a, quaternion.w / a);
}

Quaternion Inverse(const Quaternion& quaternion)
{
	Quaternion a = Conjugate(quaternion);
	float b = Norm(quaternion);
	float c = b * b;

	Quaternion d = { a.x / c, a.y / c, a.z / c, a.w / c };

	return d;
}
