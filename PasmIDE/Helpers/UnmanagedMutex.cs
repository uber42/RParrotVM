using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace PasmIDE.Helpers
{
    class UnmanagedMutex
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
        static extern SafeWaitHandle CreateMutex(IntPtr lpMutexAttributes, bool bInitialOwner, string lpName);

        [DllImport("kernel32.dll")]
        public static extern bool ReleaseMutex(SafeWaitHandle hMutex);

        private SafeWaitHandle handleValue = null;
        private IntPtr mutexAttrValue = IntPtr.Zero;
        private string nameValue = null;

        public UnmanagedMutex(string Name)
        {
            nameValue = Name;
        }

        public void Create()
        {
            if (nameValue == null && nameValue.Length == 0)
            {
                throw new ArgumentNullException("nameValue");
            }

            handleValue = CreateMutex(mutexAttrValue, true, nameValue);

            if (handleValue.IsInvalid)
            {
                Marshal.ThrowExceptionForHR(Marshal.GetHRForLastWin32Error());
            }
        }

        public SafeWaitHandle Handle
        {
            get
            {
                if (!handleValue.IsInvalid)
                {
                    return handleValue;
                }
                else
                {
                    return null;
                }
            }
        }

        public string Name
        {
            get
            {
                return nameValue;
            }
        }

        public void Release()
        {
            ReleaseMutex(handleValue);
        }
    }
}
