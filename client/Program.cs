using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace repatriator_client
{
    static class Program
    {
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            Settings.load();
            Application.Run(new ConnectionWindow());
        }
    }
}
