﻿using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Xml;

namespace Kres.Man
{
    public class Program
    {
        private static readonly log4net.ILog log = log4net.LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        private static void LoadLogConfig()
        {
            var log4netConfig = new XmlDocument();
            log4netConfig.Load(File.OpenRead("log4net.config"));

            var repo = log4net.LogManager.CreateRepository(
                Assembly.GetEntryAssembly(), typeof(log4net.Repository.Hierarchy.Hierarchy));

            log4net.GlobalContext.Properties["pid"] = Process.GetCurrentProcess().Id;

            log4net.Config.XmlConfigurator.Configure(repo, log4netConfig["log4net"]);
        }

        public static void Main(string[] args)
        {
            LoadLogConfig();
            log.Info("Main");

            log.Info("Starting UDP Server");
            UdpServer.Listen();

            log.Info("Starting CoreClient Updater");
            CoreClient.Start();

            log.Info("Starting HTTP Listener");
            var listener = new Listener();
            var kresUpdater = new KresUpdater();

            log.Info("Starting Knot-Resolver Updater");
            kresUpdater.Start(listener);

            listener.Listen();


        }
    }
}