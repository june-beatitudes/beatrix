# API Reference

## Header files

- [kernel/include/ftoa.h](#file-kernelincludeftoah)
- [kernel/include/isr.h](#file-kernelincludeisrh)
- [kernel/include/logging.h](#file-kernelincludeloggingh)
- [kernel/include/processor.h](#file-kernelincludeprocessorh)
- [kernel/include/pwr.h](#file-kernelincludepwrh)
- [kernel/include/rcc.h](#file-kernelincludercch)
- [kernel/include/register_utils.h](#file-kernelincluderegister_utilsh)
- [kernel/include/rtc.h](#file-kernelincludertch)
- [kernel/include/string.h](#file-kernelincludestringh)
- [kernel/include/version.h](#file-kernelincludeversionh)

## File kernel/include/ftoa.h

_Functionality for converting floats to strings inside the kernel, used primarily for logging._

**Author:**

Juniper Beatitudes

## Structures and Types

| Type | Name |
| ---: | :--- |
| enum  | [**bea\_ftoa\_result**](#enum-bea_ftoa_result)  <br>_The result indicator of the_ `bea_ftoa`_ function._ |

## Functions

| Type | Name |
| ---: | :--- |
|  enum [**bea\_ftoa\_result**](#enum-bea_ftoa_result) | [**bea\_ftoa**](#function-bea_ftoa) (float x, char \*buf, size\_t n\_available, bool include\_positive\_sign) <br>_Fills a buffer with a string representation of a single-precision IEEE-754 floating point number._ |


## Structures and Types Documentation

### enum `bea_ftoa_result`

_The result indicator of the_ `bea_ftoa`_ function._
```c
enum bea_ftoa_result {
    BEA_FTOA_SUCCESS,
    BEA_FTOA_ENOSPACE,
    BEA_FTOA_WPRECISIONLOSS
};
```


## Functions Documentation

### function `bea_ftoa`

_Fills a buffer with a string representation of a single-precision IEEE-754 floating point number._
```c
enum bea_ftoa_result bea_ftoa (
    float x,
    char *buf,
    size_t n_available,
    bool include_positive_sign
) 
```


**Parameters:**


* `x` The number to stringify 
* `buf` The output buffer 
* `n_available` The number of characters available in the output buffer; all characters will be used 
* `include_positive_sign` Boolean indicating whether or not a sign character should be emitted if `x` is positive


**Returns:**

BEA\_FTOA\_SUCCESS Represents a fully successful run with no loss-of-precision warning 



**Returns:**

BEA\_FTOA\_ENOSPACE Indicates that there is insufficient space in the buffer to represent the integral portion of the number 



**Returns:**

BEA\_FTOA\_WPRECISIONLOSS Indicates that there are more significant digits emitted than a single-precision floating point number actually has



**Todo**

Add support for emitting INF, NaN 

Add support for scientific notation


## File kernel/include/isr.h





## Structures and Types

| Type | Name |
| ---: | :--- |
| typedef void(\* | [**isr\_t**](#typedef-isr_t)  <br>_Represents an ISR._ |



## Structures and Types Documentation

### typedef `isr_t`

_Represents an ISR._
```c
typedef void(* isr_t) (void);
```




## File kernel/include/logging.h





## Structures and Types

| Type | Name |
| ---: | :--- |
| enum  | [**bea\_log\_level\_t**](#enum-bea_log_level_t)  <br>_Kernel logging levels._ |

## Functions

| Type | Name |
| ---: | :--- |
|  bool | [**bea\_log**](#function-bea_log) (enum [**bea\_log\_level\_t**](#enum-bea_log_level_t) level, const char \*msg) <br>_Log message to whatever log output device has been specified at build time._ |


## Structures and Types Documentation

### enum `bea_log_level_t`

_Kernel logging levels._
```c
enum bea_log_level_t {
    BEA_LOG_DEBUG = 0,
    BEA_LOG_INFO,
    BEA_LOG_WARNING,
    BEA_LOG_ERROR,
    BEA_LOG_FATAL
};
```


## Functions Documentation

### function `bea_log`

_Log message to whatever log output device has been specified at build time._
```c
bool bea_log (
    enum bea_log_level_t level,
    const char *msg
) 
```


**Parameters:**


* `level` The log level 
* `msg` The log message


**Returns:**

true Logging was actually performed (device is connected and log level is set appropriately) 



**Returns:**

false Logging was not performed


## File kernel/include/processor.h





## Structures and Types

| Type | Name |
| ---: | :--- |
| enum  | [**bea\_io\_bank\_t**](#enum-bea_io_bank_t)  <br>_Represents an I/O peripheral bank._ |
| enum  | [**bea\_semihost\_reason\_t**](#enum-bea_semihost_reason_t)  <br>_Represents a semihost request type._ |

## Functions

| Type | Name |
| ---: | :--- |
|  void | [**bea\_fpu\_enable**](#function-bea_fpu_enable) () <br>_Enables the STM32WB55RGV hardware FPU. Must be called before_ _any__ floating point instructions are executed. Reference STMicroelectronics PM0214 Rev. 10, pp. 257._ |
|  int32\_t | [**bea\_semihost\_rq**](#function-bea_semihost_rq) (int32\_t reason, void \*arg) <br>_Sends a semihost request to whatever device is connected (if semihosting is enabled)_ |


## Structures and Types Documentation

### enum `bea_io_bank_t`

_Represents an I/O peripheral bank._
```c
enum bea_io_bank_t {
    BEA_APB1,
    BEA_APB2,
    BEA_APB3,
    BEA_AHB1,
    BEA_AHB2,
    BEA_AHB3
};
```

### enum `bea_semihost_reason_t`

_Represents a semihost request type._
```c
enum bea_semihost_reason_t {
    BEA_SEMIHOST_SYSWRITEZ = 0x04
};
```


## Functions Documentation

### function `bea_fpu_enable`

_Enables the STM32WB55RGV hardware FPU. Must be called before_ _any__ floating point instructions are executed. Reference STMicroelectronics PM0214 Rev. 10, pp. 257._
```c
void bea_fpu_enable () 
```

### function `bea_semihost_rq`

_Sends a semihost request to whatever device is connected (if semihosting is enabled)_
```c
int32_t bea_semihost_rq (
    int32_t reason,
    void *arg
) 
```


**Parameters:**


* `reason` The semihost request "reason", i.e. the request type 
* `arg` The semihost request argument


**Returns:**

The semihost request result from the processor/device


## File kernel/include/pwr.h







## Macros

| Type | Name |
| ---: | :--- |
| define  | [**BEA\_PWR\_BASE\_ADDR**](#define-bea_pwr_base_addr)  (uint32\_t \*)(0x58000400)<br> |
| define  | [**BEA\_PWR\_CR1\_OFFSET**](#define-bea_pwr_cr1_offset)  0x0<br> |



## Macros Documentation

### define `BEA_PWR_BASE_ADDR`

```c
#define BEA_PWR_BASE_ADDR (uint32_t *)(0x58000400)
```

### define `BEA_PWR_CR1_OFFSET`

```c
#define BEA_PWR_CR1_OFFSET 0x0
```


## File kernel/include/rcc.h






## Functions

| Type | Name |
| ---: | :--- |
|  bool | [**bea\_rcc\_enable\_peripheral**](#function-bea_rcc_enable_peripheral) (enum [**bea\_io\_bank\_t**](#enum-bea_io_bank_t) io\_bank, uint8\_t offset) <br>_Enable a specific peripheral in the RCC subsystem._ |

## Macros

| Type | Name |
| ---: | :--- |
| define  | [**BEA\_RCC\_BASE\_ADDR**](#define-bea_rcc_base_addr)  (uint32\_t \*)(0x58000000)<br> |


## Functions Documentation

### function `bea_rcc_enable_peripheral`

_Enable a specific peripheral in the RCC subsystem._
```c
bool bea_rcc_enable_peripheral (
    enum bea_io_bank_t io_bank,
    uint8_t offset
) 
```


**Parameters:**


* `io_bank` The I/O bank the peripheral is in (e.g., AHB1, AHB2, etc.) 
* `offset` The offset of the peripheral in the bank's register. APB1 has two registers; to access the second register, pass in the offset in that register plus 32 


**Returns:**

true Peripheral enable successful (within reason to check) 



**Returns:**

false Peripheral enable unsuccessful

## Macros Documentation

### define `BEA_RCC_BASE_ADDR`

```c
#define BEA_RCC_BASE_ADDR (uint32_t *)(0x58000000)
```


## File kernel/include/register_utils.h






## Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**bea\_get\_reg\_bits**](#function-bea_get_reg_bits) (volatile uint32\_t \*base\_addr, uint8\_t msb, uint8\_t lsb) <br>_Get bits in a hardware memory-mapped register (assumed to be 32 bits wide). Reading from an incorrect memory address is undefined behavior and may trigger a hard fault or other interrupt._ |
|  void | [**bea\_set\_reg\_bits**](#function-bea_set_reg_bits) (volatile uint32\_t \*base\_addr, uint8\_t msb, uint8\_t lsb, uint32\_t value) <br>_Set bits in a hardware memory-mapped register (assumed to be 32 bits wide). Writing to an incorrect memory address is undefined behavior and may trigger a hard fault or other interrupt._ |



## Functions Documentation

### function `bea_get_reg_bits`

_Get bits in a hardware memory-mapped register (assumed to be 32 bits wide). Reading from an incorrect memory address is undefined behavior and may trigger a hard fault or other interrupt._
```c
uint32_t bea_get_reg_bits (
    volatile uint32_t *base_addr,
    uint8_t msb,
    uint8_t lsb
) 
```


**Parameters:**


* `base_addr` The base memory address of the register 
* `msb` The "highest" bit (inclusive) to read, zero-indexed, big-endian 
* `lsb` The "lowest" bit (inclusive) to read, zero-indexed, big-endian
### function `bea_set_reg_bits`

_Set bits in a hardware memory-mapped register (assumed to be 32 bits wide). Writing to an incorrect memory address is undefined behavior and may trigger a hard fault or other interrupt._
```c
void bea_set_reg_bits (
    volatile uint32_t *base_addr,
    uint8_t msb,
    uint8_t lsb,
    uint32_t value
) 
```


**Parameters:**


* `base_addr` The base memory address of the register 
* `msb` The "highest" bit (inclusive) to set, zero-indexed, big-endian 
* `lsb` The "lowest" bit (inclusive) to set, zero-indexed, big-endian 
* `value` The value to write into that range of the register. Only the lowest `(1 + msb - lsb)` bits will be written, all others will be discarded


## File kernel/include/rtc.h





## Structures and Types

| Type | Name |
| ---: | :--- |
| struct | [**bea\_datetime**](#struct-bea_datetime) <br>_Represents a single point in time in the Gregorian calendar. Years begin at 2000 (since the RTC hardware can't represent anything earlier than that), and hours are in 24-hour time. Always in UTC because the IANA timezone database is in itself bigger than the amount of flash we have to work with._ |
| enum  | [**bea\_dotw**](#enum-bea_dotw)  <br>_Represents a day of the week in the Gregorian calendar._ |
| enum  | [**bea\_month**](#enum-bea_month)  <br>_Represents a month in the Gregorian calendar._ |
| enum  | [**bea\_rtc\_clksrc**](#enum-bea_rtc_clksrc)  <br>_Represents a clock source for the RTC subsystem to use (see RM0434 pp. 263)_ |

## Functions

| Type | Name |
| ---: | :--- |
|  bool | [**bea\_datetime\_is\_valid**](#function-bea_datetime_is_valid) (struct [**bea\_datetime**](#struct-bea_datetime) datetime) <br>_Checks whether a_ `struct  bea_datetime`_ represents a valid point in time in the Gregorian calendar._ |
|  void | [**bea\_datetime\_to\_kstrz**](#function-bea_datetime_to_kstrz) (const struct [**bea\_datetime**](#struct-bea_datetime) datetime, char \*buf) <br>_Fills a buffer of at least 31 chars (please just use_ `BEA_DATETIME_KSTRZ_LEN`_) with a null terminated string in BEATRIX kernel format._ |
|  bool | [**bea\_datetime\_to\_unix**](#function-bea_datetime_to_unix) (struct [**bea\_datetime**](#struct-bea_datetime) datetime, uint64\_t timestamp\_out) <br>_converts a_ `bea_datetime`_ to a Unix timestamp (seconds since the beginning of the year 1970)_ |
|  struct [**bea\_datetime**](#struct-bea_datetime) | [**bea\_rtc\_get\_datetime**](#function-bea_rtc_get_datetime) () <br>_Reads the current date and time according to the RTC._ |
|  bool | [**bea\_rtc\_initialize**](#function-bea_rtc_initialize) (enum [**bea\_rtc\_clksrc**](#enum-bea_rtc_clksrc) clock\_source, struct [**bea\_datetime**](#struct-bea_datetime) start\_time, uint32\_t subsec\_frequency) <br>_Initialize the RTC subsystem._ |
|  bool | [**bea\_unix\_to\_datetime**](#function-bea_unix_to_datetime) (uint64\_t timestamp, struct [**bea\_datetime**](#struct-bea_datetime) \*datetime\_out) <br>_Converts a standard Unix timestamp (seconds since the beginning of the year 1970) to a_ `bea_datetime` |

## Macros

| Type | Name |
| ---: | :--- |
| define  | [**BEA\_DATETIME\_KSTRZ\_LEN**](#define-bea_datetime_kstrz_len)  31<br> |
| define  | [**BEA\_RCC\_BDCR\_OFFSET**](#define-bea_rcc_bdcr_offset)  0x024<br> |
| define  | [**BEA\_RTC\_BASE\_ADDR**](#define-bea_rtc_base_addr)  (uint32\_t \*)(0x40002800)<br> |
| define  | [**BEA\_RTC\_DR\_OFFSET**](#define-bea_rtc_dr_offset)  0x01<br> |
| define  | [**BEA\_RTC\_ISR\_OFFSET**](#define-bea_rtc_isr_offset)  0x03<br> |
| define  | [**BEA\_RTC\_PRER\_OFFSET**](#define-bea_rtc_prer_offset)  0x04<br> |
| define  | [**BEA\_RTC\_SSR\_OFFSET**](#define-bea_rtc_ssr_offset)  0x0A<br> |
| define  | [**BEA\_RTC\_TR\_OFFSET**](#define-bea_rtc_tr_offset)  0x00<br> |
| define  | [**BEA\_RTC\_WPR\_OFFSET**](#define-bea_rtc_wpr_offset)  0x09<br> |

## Structures and Types Documentation

### struct `bea_datetime`

_Represents a single point in time in the Gregorian calendar. Years begin at 2000 (since the RTC hardware can't represent anything earlier than that), and hours are in 24-hour time. Always in UTC because the IANA timezone database is in itself bigger than the amount of flash we have to work with._

Variables:

-  uint8\_t day  

-  enum [**bea\_dotw**](#enum-bea_dotw) dotw  

-  uint8\_t hour  

-  uint8\_t minute  

-  enum [**bea\_month**](#enum-bea_month) month  

-  float second  

-  uint8\_t year  <br>_Years since 2000._

### enum `bea_dotw`

_Represents a day of the week in the Gregorian calendar._
```c
enum bea_dotw {
    BEA_DOTW_MONDAY = 0,
    BEA_DOTW_TUESDAY,
    BEA_DOTW_WEDNESDAY,
    BEA_DOTW_THURSDAY,
    BEA_DOTW_FRIDAY,
    BEA_DOTW_SATURDAY,
    BEA_DOTW_SUNDAY
};
```

### enum `bea_month`

_Represents a month in the Gregorian calendar._
```c
enum bea_month {
    BEA_MONTH_JANUARY = 0,
    BEA_MONTH_FEBRUARY,
    BEA_MONTH_MARCH,
    BEA_MONTH_APRIL,
    BEA_MONTH_MAY,
    BEA_MONTH_JUNE,
    BEA_MONTH_JULY,
    BEA_MONTH_AUGUST,
    BEA_MONTH_SEPTEMBER,
    BEA_MONTH_OCTOBER,
    BEA_MONTH_NOVEMBER,
    BEA_MONTH_DECEMBER
};
```

### enum `bea_rtc_clksrc`

_Represents a clock source for the RTC subsystem to use (see RM0434 pp. 263)_
```c
enum bea_rtc_clksrc {
    BEA_RTC_CLKSRC_LSE,
    BEA_RTC_CLKSRC_LSI,
    BEA_RTC_CLKSRC_HSE
};
```


## Functions Documentation

### function `bea_datetime_is_valid`

_Checks whether a_ `struct  bea_datetime`_ represents a valid point in time in the Gregorian calendar._
```c
bool bea_datetime_is_valid (
    struct bea_datetime datetime
) 
```


**Parameters:**


* `datetime` The datetime to check 


**Returns:**

true The represented datetime is valid 



**Returns:**

false The represented datetime is invalid
### function `bea_datetime_to_kstrz`

_Fills a buffer of at least 31 chars (please just use_ `BEA_DATETIME_KSTRZ_LEN`_) with a null terminated string in BEATRIX kernel format._
```c
void bea_datetime_to_kstrz (
    const struct bea_datetime datetime,
    char *buf
) 
```


The BEATRIX kernel format is loosely based on ISO and is "WDY MON DY YEAR
HR:MN:SECOND UTC", with seconds being given with 5 significant figures of precision and a decimal point.



**Parameters:**


* `datetime` The datetime to use 
* `buf` The buffer to fill
### function `bea_datetime_to_unix`

_converts a_ `bea_datetime`_ to a Unix timestamp (seconds since the beginning of the year 1970)_
```c
bool bea_datetime_to_unix (
    struct bea_datetime datetime,
    uint64_t timestamp_out
) 
```


**Parameters:**


* `datetime` The datetime in `bea_datetime` format
* `timestamp_out` A pointer to an uninitialized `uint64_t`, whose value will be filled in by the function


**Returns:**

true The conversion was successful 



**Returns:**

false The conversion was unsuccessful because `datetime` is invalid
### function `bea_rtc_get_datetime`

_Reads the current date and time according to the RTC._
```c
struct bea_datetime bea_rtc_get_datetime () 
```


**Returns:**

struct [**bea\_datetime**](#struct-bea_datetime) The returned date and time values
### function `bea_rtc_initialize`

_Initialize the RTC subsystem._
```c
bool bea_rtc_initialize (
    enum bea_rtc_clksrc clock_source,
    struct bea_datetime start_time,
    uint32_t subsec_frequency
) 
```


**Parameters:**


* `clock_source` Clock source for the RTC subsystem to use (see STMicroelectronics RM0434 pp. 263) 
* `start_time` The time at initialization 
* `subsec_frequency` The maximum frequency (in Hertz) the RTC can resolve. Validity depends on the clock source; reference page mentioned above 


**Returns:**

true No error encountered 



**Returns:**

false Error encountered



**Todo**

Add timeout for INITF flag 

Add timeout for initialization completion 

Add less opaque error return
### function `bea_unix_to_datetime`

_Converts a standard Unix timestamp (seconds since the beginning of the year 1970) to a_ `bea_datetime`
```c
bool bea_unix_to_datetime (
    uint64_t timestamp,
    struct bea_datetime *datetime_out
) 
```


**Parameters:**


* `timestamp` The timestamp in Unix format 
* `datetime_out` A pointer to an uninitialized `bea_datetime` object, which will be filled out by the function


**Returns:**

true The conversion was successful 



**Returns:**

false The conversion was unsuccessful due to a mismatch in representable ranges

## Macros Documentation

### define `BEA_DATETIME_KSTRZ_LEN`

```c
#define BEA_DATETIME_KSTRZ_LEN 31
```

### define `BEA_RCC_BDCR_OFFSET`

```c
#define BEA_RCC_BDCR_OFFSET 0x024
```

### define `BEA_RTC_BASE_ADDR`

```c
#define BEA_RTC_BASE_ADDR (uint32_t *)(0x40002800)
```

### define `BEA_RTC_DR_OFFSET`

```c
#define BEA_RTC_DR_OFFSET 0x01
```

### define `BEA_RTC_ISR_OFFSET`

```c
#define BEA_RTC_ISR_OFFSET 0x03
```

### define `BEA_RTC_PRER_OFFSET`

```c
#define BEA_RTC_PRER_OFFSET 0x04
```

### define `BEA_RTC_SSR_OFFSET`

```c
#define BEA_RTC_SSR_OFFSET 0x0A
```

### define `BEA_RTC_TR_OFFSET`

```c
#define BEA_RTC_TR_OFFSET 0x00
```

### define `BEA_RTC_WPR_OFFSET`

```c
#define BEA_RTC_WPR_OFFSET 0x09
```


## File kernel/include/string.h






## Functions

| Type | Name |
| ---: | :--- |
|  void | [**bea\_strncpy**](#function-bea_strncpy) (const char \*from, char \*to, size\_t n) <br>_Copies_ `n`_ characters from a character buffer._ |



## Functions Documentation

### function `bea_strncpy`

_Copies_ `n`_ characters from a character buffer._
```c
void bea_strncpy (
    const char *from,
    char *to,
    size_t n
) 
```


**Parameters:**


* `from` The buffer to copy from 
* `to` The buffer to copy into 
* `n` The number of characters to copy


## File kernel/include/version.h





## Structures and Types

| Type | Name |
| ---: | :--- |
| struct | [**bea\_version**](#struct-bea_version) <br>_A version of BEATRIX is uniquely given by its Git commit hash and the major version name (conventionally the name of a Pigeon Pit song)_ |


## Macros

| Type | Name |
| ---: | :--- |
| define  | [**BEA\_GIT\_HASH\_LEN**](#define-bea_git_hash_len)  40<br> |
| define  | [**BEA\_VERSION\_NAME\_MAXLEN**](#define-bea_version_name_maxlen)  31<br> |

## Structures and Types Documentation

### struct `bea_version`

_A version of BEATRIX is uniquely given by its Git commit hash and the major version name (conventionally the name of a Pigeon Pit song)_

Variables:

-  char git_hash  

-  char name  



## Macros Documentation

### define `BEA_GIT_HASH_LEN`

```c
#define BEA_GIT_HASH_LEN 40
```

### define `BEA_VERSION_NAME_MAXLEN`

```c
#define BEA_VERSION_NAME_MAXLEN 31
```


