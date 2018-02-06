﻿using System.IO;
using System.Reflection;
using System.Xml;

namespace Kres.Man
{
    public class Program
    {
        private static readonly log4net.ILog log = log4net.LogManager.GetLogger(typeof(Program));

        private static void LoadLogConfig()
        {
            var log4netConfig = new XmlDocument();
            log4netConfig.Load(File.OpenRead("log4net.config"));

            var repo = log4net.LogManager.CreateRepository(
                Assembly.GetEntryAssembly(), typeof(log4net.Repository.Hierarchy.Hierarchy));

            log4net.Config.XmlConfigurator.Configure(repo, log4netConfig["log4net"]);
        }

        public static void Main(string[] args)
        {
            LoadLogConfig();
            log.Info("Main");

            var listener = new Listener();
            listener.Listen();

            UdpServer.Listen();

        }
    }
}