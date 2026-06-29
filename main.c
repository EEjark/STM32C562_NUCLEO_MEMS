/**
 ******************************************************************************
 * @file    main.c
 * @brief   Main entry — two-phase init (mirrors DataLogFusion main.c):
 *   Phase 1: MEMS_Init()   — hardware + protocol init
 *   Phase 2: MEMS_Process() — superloop
 ******************************************************************************
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 ******************************************************************************
 */
#include "main.h"
#include "app_mems_protocol.h"

int main(void)
{
  if (mx_system_init() != SYSTEM_OK)
   { for (;;); }

  MEMS_Init();               /* Phase 1 — all HW init + protocol state */

  for (;;)
  {
    MEMS_Process();          /* Phase 2 — command dispatch + 100 Hz stream */
  }
}