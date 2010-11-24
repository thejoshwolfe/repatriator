﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace repatriator_client
{
    public class ConnectionSettings
    {
        public string url;
        public int port;
        public string username;
        public string password = ""; // empty string means no saved password
    }
}