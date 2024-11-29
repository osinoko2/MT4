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

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s\n", label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + (row + 1) * kRowHeight, "%6.03f", matrix.m[row][column]);
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

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 b;
	b.x = (vector.x * matrix.m[0][0]) + (vector.y * matrix.m[1][0]) + (vector.z * matrix.m[2][0]) + (1.0f * matrix.m[3][0]);
	b.y = (vector.x * matrix.m[0][1]) + (vector.y * matrix.m[1][1]) + (vector.z * matrix.m[2][1]) + (1.0f * matrix.m[3][1]);
	b.z = (vector.x * matrix.m[0][2]) + (vector.y * matrix.m[1][2]) + (vector.z * matrix.m[2][2]) + (1.0f * matrix.m[3][2]);
	float w = (vector.x * matrix.m[0][3]) + (vector.y * matrix.m[1][3]) + (vector.z * matrix.m[2][3]) + (1.0f * matrix.m[3][3]);
	assert(w != 0.0f);
	b.x /= w;
	b.y /= w;
	b.z /= w;
	return b;
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
// 任意軸回転を表すQuaternionの生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);
// 球面線形補間
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

Quaternion Add(const Quaternion& q1, const Quaternion& q2) { return Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w); }

Quaternion Multiply(float scaler, const Quaternion& q) { return Quaternion(scaler * q.x, scaler * q.y, scaler * q.z, scaler * q.w); }

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};
	Quaternion rotation0 = MakeRotateAxisAngleQuaternion({0.71f, 0.71f, 0.0f}, 0.3f);
	Quaternion rotation1 = {-rotation0.x, -rotation0.y, -rotation0.z, -rotation0.w};

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

		Quaternion interpolate0 = Slerp(rotation0, rotation1, 0.0f);
		Quaternion interpolate1 = Slerp(rotation0, rotation1, 0.3f);
		Quaternion interpolate2 = Slerp(rotation0, rotation1, 0.5f);
		Quaternion interpolate3 = Slerp(rotation0, rotation1, 0.7f);
		Quaternion interpolate4 = Slerp(rotation0, rotation1, 1.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		QuaternionScreenPrintf(0, kRowHeight * 0, interpolate0, ": interpolate0, Slerp(q0, q1, 0.0f)");
		QuaternionScreenPrintf(0, kRowHeight * 1, interpolate1, ": interpolate1, Slerp(q0, q1, 0.3f)");
		QuaternionScreenPrintf(0, kRowHeight * 2, interpolate2, ": interpolate2, Slerp(q0, q1, 0.5f)");
		QuaternionScreenPrintf(0, kRowHeight * 3, interpolate3, ": interpolate3, Slerp(q0, q1, 0.7f)");
		QuaternionScreenPrintf(0, kRowHeight * 4, interpolate4, ": interpolate4, Slerp(q0, q1, 1.0f)");
		

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

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle)
{
	float a = sinf(angle / 2.0f);

	return Quaternion(axis.x * a, axis.y * a, axis.z * a, cosf(angle / 2.0f));
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion)
{
	Quaternion r = { vector.x, vector.y, vector.z, 0.0f };
	Quaternion a = Conjugate(quaternion);
	Quaternion b = Multiply(quaternion, Multiply(r, a));

	return Vector3(b.x, b.y, b.z);
}

Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion)
{
	float x = quaternion.x;
	float y = quaternion.y;
	float z = quaternion.z;
	float w = quaternion.w;

	Matrix4x4 R;

	R.m[0][0] = w * w + x * x - y * y - z * z;
	R.m[0][1] = (x * y + w * z) * 2.0f;
	R.m[0][2] = (x * z - w * y) * 2.0f;
	R.m[0][3] = 0;

	R.m[1][0] = (x * y - w * z) * 2.0f;
	R.m[1][1] = w * w - x * x + y * y - z * z;
	R.m[1][2] = (y * z + w * x) * 2.0f;
	R.m[1][3] = 0;

	R.m[2][0] = (x * z + w * y) * 2.0f;
	R.m[2][1] = (y * z - w * x) * 2.0f;
	R.m[2][2] = w * w - x * x - y * y + z * z;
	R.m[2][3] = 0;

	R.m[3][0] = 0;
	R.m[3][1] = 0;
	R.m[3][2] = 0;
	R.m[3][3] = 1;

	return R;
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
	Quaternion q = q0;
	float EPSILON = 0.0005f;
	if (dot < 0){
		q = Quaternion(-q0.x, -q0.y, -q0.z, -q0.w);
		dot = -dot;
	}

	if (dot >= 1.0f - EPSILON){
		return Add(Multiply(1.0f - t, q), Multiply(t, q1));
	}
	// なす角を求める
	float theta = std::acos(dot);

	// thetaとsinを使って補間係数scale0, scale1を求める
	float scale0 = sinf((1.0f - t) * theta) / sinf(theta);
	float scale1 = sinf(t * theta) / sinf(theta);

	// それぞれの補間係数を利用して補間後のQuaternionを求める

	return Add(Multiply(scale0, q), Multiply(scale1, q1));
}
