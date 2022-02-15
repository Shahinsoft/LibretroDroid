package com.shahinsoft.libretrodroid

import java.lang.RuntimeException

class RetroException(val errorCode: Int) : RuntimeException("Retro exception. Error code $errorCode")