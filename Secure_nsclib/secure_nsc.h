/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Secure_nsclib/secure_nsc.h
  * @author  MCD Application Team
  * @brief   Header for secure non-secure callable APIs list
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* USER CODE BEGIN Non_Secure_CallLib_h */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SECURE_NSC_H
#define SECURE_NSC_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>

extern void *pSecureFaultCallback;
extern void *pSecureErrorCallback;
/* Exported types ------------------------------------------------------------*/
/**
  * @brief  non-secure callback ID enumeration definition
  */
typedef enum
{
  SECURE_FAULT_CB_ID     = 0x00U, /*!< System secure fault callback ID */
  GTZC_ERROR_CB_ID       = 0x01U  /*!< GTZC secure error callback ID */
} SECURE_CallbackIDTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

//CMSE_NS_ENTRY void SECURE_RegisterCallback(int id, void *func);
void SECURE_LEDToggle(void);
//CMSE_NS_ENTRY const char* SECURE_GetMessage(void);
//CMSE_NS_ENTRY void SECURE_CopyMessage(char *buffer, size_t maxlen);
//CMSE_NS_ENTRY void SECURE_RegisterPrintCallback(void *callback);
//CMSE_NS_ENTRY void SECURE_Print(const char *msg);

void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func);
void SECURE_LEDToggle(void);

void SECURE_ComputeHMAC(uint8_t *output_digest, size_t maxlen);
void SECURE_LinearHMAC(uint8_t *output_digest, size_t maxlen);
void SECURE_ShuffledHMAC(uint8_t *output_digest, size_t maxlen);
void SECURE_TEST(uint8_t *output_digest, size_t maxlen);
void SECURE_SMARM(uint8_t *output_digest, size_t maxlen);
void SECURE_FillTest(uint8_t *buffer, size_t len);
void SECURE_CopyMessage(char* buffer, size_t maxlen);

//const char* SECURE_GetMessage(void);
//void SECURE_CopyMessage(char *buffer, size_t maxlen)
//void SECURE_RunHMAC(void);



#endif /* SECURE_NSC_H */
/* USER CODE END Non_Secure_CallLib_h */

