#ifndef _URICODE_H_
#define _URICODE_H_ 1
// 0～9 	0030～0039 	数字 	英数字
// ラテン大文字については下記説明を参照
// A～Z 	0041～005A 	ラテン大文字
// a～z 	0061～007A 	ラテン小文字
// - 	002D 	ハイフンマイナス
// . 	002E 	ピリオド 	下記説明を参照
// _ 	005F 	アンダーライン
// ~ 	007E 	チルド 	下記説明を参照
void URI_enCode(const unsigned char* sorc, unsigned char* dist);
void URI_deCode(unsigned char* data);

#endif  // _URICODE_H_