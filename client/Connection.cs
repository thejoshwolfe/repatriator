using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace repatriator_client
{
    public class Connection
    {
        public string url;
        public string port;
        public string username;
        public string password = ""; // empty string means no saved password
    }
}
