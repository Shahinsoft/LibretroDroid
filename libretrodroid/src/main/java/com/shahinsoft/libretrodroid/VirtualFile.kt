package com.shahinsoft.libretrodroid

import android.os.ParcelFileDescriptor

data class VirtualFile(val virtualPath: String, val fileDescriptor: ParcelFileDescriptor)
