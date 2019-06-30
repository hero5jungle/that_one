#pragma once
#include "../../sdk/sdk.h"
typedef struct CScreenSize_t {
  int Height;
  int Width;
} CScreenSize;

void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce );
void Intro();

extern unsigned int FocusOverlayPanel;
extern CScreenSize gScreen;
