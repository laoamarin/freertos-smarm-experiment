/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Secure/Src/secure_nsc.c
  * @author  MCD Application Team
  * @brief   This file contains the non-secure callable APIs (secure world)
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

/* USER CODE BEGIN Non_Secure_CallLib */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "secure_nsc.h"
#include <stdio.h>
#include "secure_port_macros.h"  
#include <string.h>
#include "Crypto/hmac-sha256/hmac-sha256.h"
#include "aes-gcm/aes.h"   // รวม AES CBC, CTR, ECB ไว้หมด
#include "Aesnew/aes.h"

#define SHA256_DIGEST_SIZE 32
#define BLOCK_SIZE 512 // 32 124 1024 4096
#define TOTAL_SIZE 0x40000
#define BLOCKS (TOTAL_SIZE / BLOCK_SIZE)
/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup FreeRTOS_SecureIOToggle_TrustZone
  * @{
  */

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL;   /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL;   /* Pointer to secure error callback in Non-secure */
static void (*ns_print_cb)(const char *) = NULL;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



CMSE_NS_ENTRY void SECURE_RegisterPrintCallback(void *callback)
{
    ns_print_cb = (void (*)(const char *))cmse_nsfptr_create(callback);
}

CMSE_NS_ENTRY void SECURE_Print(const char *msg)
{
    if (ns_print_cb)
    {
        ns_print_cb(msg);
    }
}

/**
  * @brief  Secure registration of non-secure callback.
  * @param  CallbackId  callback identifier
  * @param  func        pointer to non-secure function
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func)
{
  if(func != NULL)
  {
    switch(CallbackId)
    {
      case SECURE_FAULT_CB_ID:           /* SecureFault Interrupt occurred */
        pSecureFaultCallback = func;
        break;
      case GTZC_ERROR_CB_ID:             /* GTZC Interrupt occurred */
        pSecureErrorCallback = func;
        break;
      default:
        /* unknown */
        break;
    }
  }
}

/**
  * @brief  Secure treatment of non-secure push button interrupt.
  * @retval None
  */
/*CMSE_NS_ENTRY*/secureportNON_SECURE_CALLABLE void SECURE_LEDToggle(void)
{
  BSP_LED_Toggle(LED1);
}

//CMSE_NS_ENTRY void SECURE_send(uint8_t* message, int size){
//	HAL_UART_Transmit(&hlpuart1, message, size, HAL_MAX_DELAY);
//};


/*CMSE_NS_ENTRY*/
//secureportNON_SECURE_CALLABLE const char* SECURE_GetMessage(void)
//{
//
//	return "AAA test in secure";
//}


__attribute__((cmse_nonsecure_entry))
void SECURE_CopyMessage(char* buffer, size_t maxlen)
{
    if (!buffer || maxlen == 0) return;
    const char* secure_msg = "AAA test in secure";
    strncpy(buffer, secure_msg, maxlen - 1);
    buffer[maxlen - 1] = '\0';
}


static const uint8_t secure_key[] = "MySecretKey";  // Fixed test key
static hmac_sha256 secure_hmac_ctx;
static uint8_t secure_digest[SHA256_DIGEST_SIZE];


__attribute__((cmse_nonsecure_entry))
void SECURE_ComputeHMAC(uint8_t *output_digest, size_t maxlen)
{

    if (!output_digest || maxlen < SHA256_DIGEST_SIZE) return;

    __disable_irq();

    for (volatile uint32_t i = 0; i < 10000000; i++);
	__enable_irq();

    const uint8_t message[] = "Temp Temp from Secure World!";

    hmac_sha256_initialize(&secure_hmac_ctx, secure_key, strlen((const char*)secure_key));
//    hmac_sha256_update(&secure_hmac_ctx, message, strlen((const char*)message));
    hmac_sha256_finalize(&secure_hmac_ctx, NULL, 0);

    for (volatile uint32_t i = 0; i < 10000000; i++);

    memcpy(secure_digest, secure_hmac_ctx.digest, SHA256_DIGEST_SIZE);


	memcpy(output_digest, secure_digest, SHA256_DIGEST_SIZE);


}

static const uint8_t key[] = "MySecureKey123"; // Example key
static hmac_sha256 hmac;
static uint8_t memory_region[1024];     // Simulate memory to attest
static uint8_t secure_digest_1[SHA256_DIGEST_SIZE];


//__attribute__((cmse_nonsecure_entry))
//void SECURE_LinearHMAC(uint8_t *output_digest, size_t maxlen)
//{
//    if (!output_digest || maxlen < SHA256_DIGEST_SIZE) return;
//
//
//
//    // Fill memory with test data
//    for (int i = 0; i < sizeof(memory_region); i++) {
//        memory_region[i] = i & 0xFF;
//    }
//
//    hmac_sha256_initialize(&hmac, key, strlen((const char*)key));
//    hmac_sha256_update(&hmac, memory_region, sizeof(memory_region));
//    hmac_sha256_finalize(&hmac, NULL, 0);
//
//    memcpy(secure_digest_1, hmac.digest, SHA256_DIGEST_SIZE);
//
//    memcpy(output_digest,secure_digest_1, SHA256_DIGEST_SIZE);
//}


static uint8_t secure_digest_2[SHA256_DIGEST_SIZE];
uint8_t *real_memory = (uint8_t *)0x8040000;
uint8_t seed[32] = {0};
//uint8_t real_memory[TOTAL_SIZE];  // This is safe, real RAM



__attribute__((cmse_nonsecure_entry))
void SECURE_ShuffledHMAC(uint8_t *output_digest, size_t maxlen)
{

	  if (!output_digest || maxlen < SHA256_DIGEST_SIZE) return;

	    static int indices[BLOCKS];
	    for (int i = 0; i < BLOCKS; i++) indices[i] = i;
	    // Simple shuffle without srand for now


	      srand(42);

	      for (int i = BLOCKS - 1; i > 0; i--) {
	          int j = rand() % (i + 1);
	          int tmp = indices[i];
	          indices[i] = indices[j];
	          indices[j] = tmp;
	      }

	    hmac_sha256_initialize(&hmac, key, strlen((const char *)key));
	    for (int i = 0; i < BLOCKS; i++) {
	        const uint8_t *block = &real_memory[indices[i] * BLOCK_SIZE];
//	        __disable_irq();
	        hmac_sha256_update(&hmac, block, BLOCK_SIZE);
//	        __enable_irq();
	    }
	    hmac_sha256_finalize(&hmac, NULL, 0);
	    memcpy(output_digest, hmac.digest, SHA256_DIGEST_SIZE);



}

//static void derive_key16_from_challenge(uint8_t out16[16],
//                                        const uint8_t *challenge, size_t clen)
//{
//    // K = HMAC_SHA256(secret_key, challenge || fallback)
//    hmac_sha256_initialize(&hmac, (const uint8_t*)key, strlen(key));
//    if (challenge && clen) {
//        hmac_sha256_update(&hmac, challenge, clen);
//    } else {
//        // fallback: add some device-side entropy so it's never all-zero
//        uint32_t tick = (uint32_t)SysTick->VAL;
//        hmac_sha256_update(&hmac, (uint8_t*)&tick, sizeof(tick));
//    }
//    hmac_sha256_finalize(&hmac, NULL, 0);
//
//    // Use first 16 bytes as AES-128 key
//    memcpy(out16, hmac.digest, 16);
//}
//
//static int prng_uniform_int_ctr(int n,
//                                const uint8_t aes_key16[16],
//                                uint32_t *counter_io)
//{
//    // rejection sampling to avoid modulo bias
//    const uint32_t limit = 0xFFFFFFFFu - (0xFFFFFFFFu % (uint32_t)n);
//
//    for (;;) {
//        uint8_t nonce[16] = {0};    // CTR nonce/IV
//        uint8_t block[16] = {0};    // plaintext buffer (zero) -> keystream
//
//        // put the counter into the first 4 bytes of nonce (little-endian is fine)
//        memcpy(nonce, counter_io, sizeof(*counter_io));
//
//        // aes_128_ctr_encrypt encrypts 'block' in place using key+nonce
//        // We only need 16 bytes of keystream for one sample
//        uint8_t key_local[16];
//        memcpy(key_local, aes_key16, 16);  // API expects non-const in some builds
//        // encrypt 16 bytes -> block now contains keystream
//        if (aes_128_ctr_encrypt(key_local, nonce, block, sizeof(block)) != 0) {
//            // if your AES returns non-zero on error, decide how to handle
//            return 0;
//        }
//
//        // interpret first 4 bytes as random 32-bit
//        uint32_t rnd;
//        memcpy(&rnd, block, sizeof(rnd));
//        (*counter_io)++;
//
//        if (rnd < limit) {
//            return (int)(rnd % (uint32_t)n);
//        }
//        // else: try again
//    }
//}
//
//static void shuffle_secure_aes_ctr(int *arr, int n,
//                                   const uint8_t aes_key16[16])
//{
//    uint32_t ctr = 0;  // start counter at 0; increments each sample
//    for (int i = n - 1; i > 0; i--) {
//        int j = prng_uniform_int_ctr(i + 1, aes_key16, &ctr);
//        int tmp = arr[i];
//        arr[i] = arr[j];
//        arr[j] = tmp;
//    }
//}
//
//
//__attribute__((cmse_nonsecure_entry))
//void SECURE_ShuffledHMAC_secure(uint8_t *out_digest, size_t out_len,
//                                const uint8_t *challenge, size_t challenge_len)
//{
//
//    if (!out_digest || out_len < SHA256_DIGEST_SIZE) return;
//
//    // 1) indices = 0..BLOCKS-1
//    static int indices[BLOCKS];
//    for (int i = 0; i < BLOCKS; i++) indices[i] = i;
//
//    // 2) derive AES-128 key from HMAC(secret, challenge)
//    uint8_t aes_key16[16];
//    derive_key16_from_challenge(aes_key16, challenge, challenge_len);
//
//    // 3) secure shuffle (unpredictable without secret key)
//    shuffle_secure_aes_ctr(indices, BLOCKS, aes_key16);
//
//    // 4) HMAC over shuffled blocks
//    hmac_sha256_initialize(&hmac, (const uint8_t*)key, strlen(key));
//    for (int i = 0; i < BLOCKS; i++) {
//        const uint8_t *block = &real_memory[(size_t)indices[i] * BLOCK_SIZE];
////        __disable_irq();
//        hmac_sha256_update(&hmac, block, BLOCK_SIZE);
////        __enable_irq();
//    }
//    hmac_sha256_finalize(&hmac, NULL, 0);
//    memcpy(out_digest, hmac.digest, SHA256_DIGEST_SIZE);
//
//}

// ---- Key/IV derivation: HMAC(secret, challenge) -> 32B -> 16B key + 16B iv
static void derive_aes_key_iv_from_challenge(uint8_t key16[16],
                                             uint8_t iv16[16],
                                             const uint8_t *challenge,
                                             size_t clen)
{
    hmac_sha256_initialize(&hmac, (const uint8_t*)key, strlen(key));
    if (challenge && clen) {
        hmac_sha256_update(&hmac, challenge, clen);
    } else {
        // fallback entropy so it’s never constant
        uint32_t tick = (uint32_t)SysTick->VAL;
        hmac_sha256_update(&hmac, (uint8_t*)&tick, sizeof(tick));
    }
    hmac_sha256_finalize(&hmac, NULL, 0);
    memcpy(key16, hmac.digest, 16);
    memcpy(iv16,  hmac.digest + 16, 16);
}

// ---- PRNG: AES-CTR keystream -> 32-bit samples
// keep a small keystream buffer so we don’t re-encrypt every time
typedef struct {
    struct AES_ctx ctx;
    uint8_t  buf[16];
    int      idx;   // next unread byte in buf (0..16)
} ctr_prng_t;

static void prng_init(ctr_prng_t *p, const uint8_t key16[16], const uint8_t iv16[16])
{
    AES_init_ctx_iv(&p->ctx, key16, iv16);
    memset(p->buf, 0, sizeof(p->buf));
    p->idx = 16; // force refill on first use
}

static void prng_refill_block(ctr_prng_t *p)
{
    // encrypt zero block → keystream, tiny-AES increments IV internally
    uint8_t zero[16] = {0};
    memcpy(p->buf, zero, 16);
    AES_CTR_xcrypt_buffer(&p->ctx, p->buf, 16); // p->ctx.Iv auto-increments
    p->idx = 0;
}

static uint32_t prng_next_u32(ctr_prng_t *p)
{
    if (p->idx > 12) {           // not enough bytes left → refill
        prng_refill_block(p);
    }
    uint32_t v;
    memcpy(&v, &p->buf[p->idx], 4);
    p->idx += 4;
    return v;
}

// unbiased integer in [0, n)
static int prng_uniform_u32(ctr_prng_t *p, int n)
{
    // rejection sampling to avoid modulo bias
    const uint32_t lim = 0xFFFFFFFFu - (0xFFFFFFFFu % (uint32_t)n);
    for (;;) {
        uint32_t r = prng_next_u32(p);
        if (r < lim) return (int)(r % (uint32_t)n);
    }
}

// ---- Fisher–Yates using the PRNG above
static void shuffle_secure_aes_ctr(int *arr, int n,
                                   const uint8_t key16[16],
                                   const uint8_t iv16[16])
{
    ctr_prng_t prng;
    prng_init(&prng, key16, iv16);

    for (int i = n - 1; i > 0; i--) {
        int j = prng_uniform_u32(&prng, i + 1);
        int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
    }
}

// ---- Non-secure callable: secure shuffle + HMAC over blocks
__attribute__((cmse_nonsecure_entry))
void SECURE_ShuffledHMAC_secure(uint8_t *out_digest, size_t out_len,
                                const uint8_t *challenge, size_t challenge_len)
{
    if (!out_digest || out_len < SHA256_DIGEST_SIZE) return;

    // 1) indices = 0..BLOCKS-1
    static int indices[BLOCKS];
    for (int i = 0; i < BLOCKS; i++) indices[i] = i;

    // 2) derive AES key/IV from challenge
    uint8_t key16[16], iv16[16];
    derive_aes_key_iv_from_challenge(key16, iv16, challenge, challenge_len);

    // 3) secure shuffle
    shuffle_secure_aes_ctr(indices, BLOCKS, key16, iv16);

    // 4) HMAC over shuffled blocks
    hmac_sha256_initialize(&hmac, (const uint8_t*)key, strlen(key));
//    uint8_t copy[BLOCK_SIZE];
    for (int i = 0; i < BLOCKS; i++) {
        const uint8_t *blk = &real_memory[(size_t)indices[i] * BLOCK_SIZE];
        __disable_irq();
        //memcpy(copy, blk, BLOCK_SIZE);
        hmac_sha256_update(&hmac, blk, BLOCK_SIZE);

        __enable_irq();
        //hmac_sha256_update(&hmac, copy, BLOCK_SIZE);
    }
    hmac_sha256_finalize(&hmac, NULL, 0);
    memcpy(out_digest, hmac.digest, SHA256_DIGEST_SIZE);
}


__attribute__((cmse_nonsecure_entry))
void SECURE_SMARM(uint8_t *output_digest, size_t maxlen)
{
	if (!output_digest || maxlen < SHA256_DIGEST_SIZE) return;

    srand(42);  // Use fixed seed or hardware-derived one like SysTick->VAL

	// Generate shuffled indices

	int indices[TOTAL_SIZE];

	//	    for (int i = 0; i < BLOCKS; i++) indices[i] = i;

	// 128 bit challenge -> sha256_update(challenge); finalize(K) -> seed (256-bit) // H(challenge || K)

	// Use seed for AES-CTR PRNG

	// note #include <aes/aes_cbc.h> น่าจะต้องหา lib

	// ben shuffle_aes_ctr ลองใช้ตัวนี้อยู๋ใน main.c -> shuffle.c

	for (int i = TOTAL_SIZE - 1; i > 0; i--) {

	int j = rand() % (i + 1);

	int tmp = indices[i];

	indices[i] = indices[j];

	indices[j] = tmp;

	}

	hmac_sha256_initialize(&hmac, key, strlen((const char )key));

	for (int i = 0; i < BLOCKS; i++) {

	const uint8_t *block = &real_memory[indices[i] * BLOCK_SIZE];

	//	        __disable_irq();

	hmac_sha256_update(&hmac, block, BLOCK_SIZE);

	//	        __enable_irq();



	}

	hmac_sha256_finalize(&hmac, NULL, 0);

	memcpy(output_digest,hmac.digest, SHA256_DIGEST_SIZE);
}

__attribute__((cmse_nonsecure_entry))
void SECURE_LinearHMAC(uint8_t *output_digest, size_t maxlen)
{


    __disable_irq();
    if (!output_digest || maxlen < SHA256_DIGEST_SIZE) {
        __enable_irq();
        return;
    }

    // Optional: Fill real_memory with known data for consistent testing
    for (int i = 0; i < TOTAL_SIZE; i++) {
        real_memory[i] = i & 0xFF;  // test pattern
    }

    hmac_sha256_initialize(&hmac, key, strlen((const char *)key));

    // Process blocks sequentially (not shuffled)
    for (int i = 0; i < BLOCKS; i++) {
        const uint8_t *block = &real_memory[i * BLOCK_SIZE];
        hmac_sha256_update(&hmac, block, BLOCK_SIZE);
    }

    hmac_sha256_finalize(&hmac, NULL, 0);
    memcpy(output_digest, hmac.digest, SHA256_DIGEST_SIZE);


    __enable_irq();
}


__attribute__((cmse_nonsecure_entry))
void SECURE_TEST(uint8_t *output_digest, size_t maxlen)
{
	 if (!output_digest || maxlen < SHA256_DIGEST_SIZE) return;

	    // Fill real_memory ifจำเป็น (ทำแบบ linear)
	    for (int i = 0; i < TOTAL_SIZE * BLOCK_SIZE; i++) {
	        real_memory[i] = i & 0xFF;
	    }

	    // 1. Initialize indices
	    int indices[BLOCKS];
	    for (int i = 0; i < BLOCKS; i++) indices[i] = i;

	    srand(42);

	    // 2. Shuffle indices
	    for (int i = BLOCKS - 1; i > 0; i--) {
	        int j = rand() % (i + 1);
	        int tmp = indices[i];
	        indices[i] = indices[j];
	        indices[j] = tmp;
	    }

	    hmac_sha256_initialize(&hmac, key, strlen((const char *)key));

	    for (int i = 0; i < BLOCKS; i++) {
	        const uint8_t *block = &real_memory[indices[i] * BLOCK_SIZE];
	        hmac_sha256_update(&hmac, block, BLOCK_SIZE);
	    }

	    hmac_sha256_finalize(&hmac, NULL, 0);
	    memcpy(output_digest, hmac.digest, SHA256_DIGEST_SIZE);
}


static void LocalTest_HMAC(void)
{
    const uint8_t message[] = "Hello from Secure World!";
    const uint8_t key[] = "MySecretKey";
    uint8_t digest[32];

    hmac_sha256_get(digest, message, strlen((const char*)message), key, strlen((const char*)key));

    // Output result via Print callback if available
    if (ns_print_cb)
    {
        SECURE_Print("[Secure] HMAC result:");
        for (int i = 0; i < 32; i++)
        {
            char buf[8];
            snprintf(buf, sizeof(buf), " %02X", digest[i]);
            SECURE_Print(buf);
        }
        SECURE_Print("\r\n");
    }

    // Or just toggle LED
    BSP_LED_Toggle(LED1);
}



//__attribute__((cmse_nonsecure_entry))
//void SECURE_ComputeHMAC(uint8_t *output_digest, size_t maxlen)
//{
//    if (!output_digest || maxlen < SHA256_DIGEST_SIZE) {
//        return;
//    }
//
//    const uint8_t message[] = "Hello from Secure World!";
//    const uint8_t key[] = "MySecretKey";
//
//    hmac_sha256 hmac;
//
//    // 1️⃣ Initialize with key
//    hmac_sha256_initialize(&hmac, key, strlen((const char*)key));
//
//    // 2️⃣ Update with message
//    hmac_sha256_update(&hmac, message, strlen((const char*)message));
//
//    // 3️⃣ Finalize to compute digest
//    hmac_sha256_finalize(&hmac, NULL, 0);
//
//    // 4️⃣ Copy digest back to NonSecure buffer
//    memcpy(output_digest, hmac.digest, SHA256_DIGEST_SIZE);
//}



__attribute__((cmse_nonsecure_entry))
void SECURE_FillTest(uint8_t *buffer, size_t len)
{

	BSP_LED_Toggle(LED1);

    if (!buffer || len < 4) return;

    buffer[0] = 0xDE;
    buffer[1] = 0xAD;
    buffer[2] = 0xBE;
    buffer[3] = 0xEF;
}





/**
  * @}
  */

/**
  * @}
  */
/* USER CODE END Non_Secure_CallLib */

