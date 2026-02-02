/* Eolithic1
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic1.LICENSE for details.
 */

namespace EolithicHelper
{
    class Program
    {
        private const int UNUSED_COMPONENT_RANGE = 20000;

        private const string FILE_COMPONENTS = "components.txt";
        private const string FILE_RESOURCE = "resources.txt";
        private const string FILE_TEMPLATE_RESOURCE_HEADER = "Template_RESOURCE.h";
        private const string FILE_TEMPLATE_RESOURCE_DEFINITIONS = "Template_RESOURCE.c";
        private const string DIR_HELPER_RESOURCES = "helper_resources";
        private const string DIR_SRC = "src";
        private const string DIR_ENGINE = "engine";
        private const string DIR_GAME = "game";
        private const string DIR_RESOURCES = "resources";
        private const string DIR_GAMESTATE = "gamestate";
        private const string KEYWORD_INCLUDE_PATH = "%strIncludePath%";
        private const string KEYWORD_RESOURCE = "%strResource%";
        private const string KEYWORD_RESOURCE_LOWER = "%strResource_lower%";
        private const string KEYWORD_RESOURCE_MANAGER = "%strResourceManager%";

        class Config
        {
        }

        static void PrintHelpLines()
        {
            Console.WriteLine("Welcome to Eolithic Helper!");
            Console.WriteLine("[C] - Generate Components Boilerplate From " + FILE_COMPONENTS);
            //Console.WriteLine("[R] - Generate Resource Boilerplate From " + FILE_RESOURCE);
            Console.WriteLine("[P] - Exit");
        }
        static void Main(string[] args)
        {
            PrintHelpLines();

            Loop();
        }
        static void Loop()
        {
            while (true)
            {
                if (!Console.KeyAvailable)
                {
                    continue;
                }

                ConsoleKey key = Console.ReadKey().Key;
                if (key == ConsoleKey.C)
                {
                    GenerateComponentBoilerplate();
                }
                else if (key == ConsoleKey.R)
                {
                    //GenerateResourceBoilerPlate();
                }
                else if (key == ConsoleKey.P)
                {
                    Environment.Exit(0);
                }
            }
        }
        static void GenerateComponentBoilerplate()
        {
            Console.WriteLine("Reading component file");

            string pathForFileComponents = Path.Combine(DIR_HELPER_RESOURCES, FILE_COMPONENTS);
            if (!File.Exists(pathForFileComponents))
            {
                Console.WriteLine("Unable to open component file");
                return;
            }

            int unusedCounter = 0;
            int usedCounter = 0;
            string[] componentFileContents = File.ReadAllLines(pathForFileComponents);

            List<string> gameStateDataHelperList = new List<string>();
            gameStateDataHelperList.Add("\n");
            gameStateDataHelperList.Add("#define GSD_INITIAL_SIZE 1\n");
            gameStateDataHelperList.Add("\n");
            gameStateDataHelperList.Add("void GameStateDataHelper_InitAllComponentPacks(GameStateData* gsd)\n");
            gameStateDataHelperList.Add("{\n");

            List<string> componentTypeHeaderList = new List<string>();
            componentTypeHeaderList.Add( "//THIS CODE IS AUTOMATICALLY GENERATED, DO NOT EDIT!\n\n");
            componentTypeHeaderList.Add("#pragma once\n");
            componentTypeHeaderList.Add("\n");
            componentTypeHeaderList.Add("REPLACEME");
            componentTypeHeaderList.Add("\n");
            componentTypeHeaderList.Add("typedef enum ComponentType\n");
            componentTypeHeaderList.Add("{\n");

            string includeThese = "";

            List<string> listOfComponentNames = new List<string>();
            List<string> listOfComponentEnumNames = new List<string>();

            int componentTypeLen = 0;
            foreach (string componentInFile in componentFileContents)
            {
                Console.WriteLine("Component in file: " + componentInFile);
                string component = componentInFile;
                bool isUnused = (component[0] == '^');
                if (isUnused)
                {
                    component = component.Remove(0, 1);
                    Console.WriteLine("Component " + component + " is unused but needed for compilation, adding to enum only.");
                }
                int initialSize = 1;
                int indexOfEqual = component.IndexOf('=');
                if (indexOfEqual >= 0)
                {
                    string newInitialSize = component.Substring(indexOfEqual + 1, component.Length - (indexOfEqual + 1));
                    int.TryParse(newInitialSize, out initialSize);
                    if (initialSize < 1)
                    {
                        initialSize = 1;
                    }
                    component = component.Substring(0, indexOfEqual);
                    Console.WriteLine("Component " + component + " has set size of " + initialSize.ToString());
                }

                string componentEnumName = "C_" + component;

                listOfComponentNames.Add(component);
                listOfComponentEnumNames.Add(componentEnumName);

                if (!isUnused)
                {
                    string fileWithStruct = FindFileWithStruct(component);
                    if (fileWithStruct == null)
                    {
                        Console.WriteLine("Unable to find: " + component + " (missing typedef perhaps?)");
                        continue;
                    }

                    string fileLocation = fileWithStruct.Replace('\\', '/');
                    /*if (fileLocation.StartsWith("src/game/")) //Old location
                    {
                        fileLocation = fileLocation.Replace("src/game/", "../../game/");
                    }
                    else
                    {
                        fileLocation = fileLocation.Replace("src/engine/", "../");
                    }*/
                    if (fileLocation.StartsWith("src/engine/")) //New location
                    {
                        fileLocation = fileLocation.Replace("src/engine/", "../engine/");
                    }
                    else
                    {
                        fileLocation = fileLocation.Replace("src/game/", "");
                    }
                    includeThese += "#include " + '"' + fileLocation + '"' + '\n';

                    Console.WriteLine("Found " + component + " at " + fileWithStruct);

                    gameStateDataHelperList.Add("    " + "ComponentPack_Init(" + componentEnumName + ", &gsd->mComponentPacks[" + componentEnumName + "], sizeof(" + component + "), " + initialSize.ToString() + ");\n");
                }

                int counterToPrintWithComponent = usedCounter;
                if (isUnused)
                {
                    counterToPrintWithComponent = (unusedCounter + UNUSED_COMPONENT_RANGE);
                }
                componentTypeHeaderList.Add("    " + componentEnumName + " = " + counterToPrintWithComponent + ",\n");

                if (isUnused)
                {
                    unusedCounter += 1;
                }
                else
                {
                    usedCounter += 1;
                    componentTypeLen += 1;
                }

                Console.WriteLine("Generating boiler plate for " + component);
            }

            componentTypeHeaderList[3] = "#define COMPONENT_TYPE_UNUSED_COMPONENT_RANGE " + UNUSED_COMPONENT_RANGE.ToString() + "\n#define COMPONENT_TYPE_LEN " + componentTypeLen + "\n";

            gameStateDataHelperList.Add("}\n");
            gameStateDataHelperList.Insert(0, "//THIS CODE IS AUTOMATICALLY GENERATED, DO NOT EDIT!\n\n#include \"../engine/gamestate/GameStateDataHelper.h\"\n#include \"ComponentType.h\"\n\n" + includeThese);

            componentTypeHeaderList.Add("} ComponentType;\n\n");
            componentTypeHeaderList.Add("const char* ComponentType_GetComponentName(ComponentType ctype);");

            {
                string componentTypeHeaderPath = Path.Combine(DIR_SRC, DIR_GAME, "ComponentType.h");
                string componentTypeHeaderString = "";
                for(int i = 0; i < componentTypeHeaderList.Count; i += 1)
                {
                    componentTypeHeaderString += componentTypeHeaderList[i];
                }
                File.WriteAllText(componentTypeHeaderPath, componentTypeHeaderString);
                Console.WriteLine("Wrote component types header to: " + componentTypeHeaderPath);
            }

            {
                List<string> componentTypeBody = new List<string>();
                componentTypeBody.Add("//THIS CODE IS AUTOMATICALLY GENERATED, DO NOT EDIT!\n\n");
                componentTypeBody.Add("#include \"ComponentType.h\"\n\n");
                componentTypeBody.Add("const char* ComponentType_GetComponentName(ComponentType ctype)\n");
                componentTypeBody.Add("{\n");
                componentTypeBody.Add("    switch (ctype)\n");
                componentTypeBody.Add("    {\n");
                for(int i = 0; i < listOfComponentNames.Count; i += 1)
                {
                    componentTypeBody.Add("    case " + listOfComponentEnumNames[i] + ":\n");
                    componentTypeBody.Add("        return " + '"' + listOfComponentNames[i] + '"' + ";\n");
                }
                componentTypeBody.Add("    default:\n");
                componentTypeBody.Add("        return " + '"' + "NOT_A_VALID_COMPONENT" + '"' + ";\n");
                componentTypeBody.Add("    }\n");
                componentTypeBody.Add("}\n");
                string componentTypeBodyPath = Path.Combine(DIR_SRC, DIR_GAME, "ComponentType.c");
                string componentTypeBodyString = "";
                for (int i = 0; i < componentTypeBody.Count; i += 1)
                {
                    componentTypeBodyString += componentTypeBody[i];
                }
                File.WriteAllText(componentTypeBodyPath, componentTypeBodyString);
                Console.WriteLine("Wrote component types body to: " + componentTypeBodyPath);
            }


            {
                //string gameStateDataHelperPath = Path.Combine(DIR_SRC, DIR_ENGINE, DIR_GAMESTATE, "GameStateDataHelper.c"); //OLD LOCATION
                string gameStateDataHelperPath = Path.Combine(DIR_SRC, DIR_GAME, "GameStateDataHelper.c");
                string gameStateDataHelperString = "";
                for (int i = 0; i < gameStateDataHelperList.Count; i += 1)
                {
                    gameStateDataHelperString += gameStateDataHelperList[i];
                }
                File.WriteAllText(gameStateDataHelperPath, gameStateDataHelperString);
                Console.WriteLine("Wrote game state data helper to: " + gameStateDataHelperPath);
            }

            Console.WriteLine("Completed creating component boiler plate!");

            PrintHelpLines();
        }
        static void WriteTemplate(string fromPath, string toPath, string replaceForInclude, string replaceForResource, string replaceForResourceManager)
        {
            string contents = File.ReadAllText(fromPath);
            contents = contents.Replace(KEYWORD_INCLUDE_PATH, replaceForInclude);
            contents = contents.Replace(KEYWORD_RESOURCE, replaceForResource);
            contents = contents.Replace(KEYWORD_RESOURCE_LOWER, replaceForResource.ToLowerInvariant());
            contents = contents.Replace(KEYWORD_RESOURCE_MANAGER, replaceForResourceManager);
            File.WriteAllText(toPath, contents);
            Console.WriteLine("Generated boiler plate for " + toPath);
        }
        static void GenerateResourceBoilerPlate()
        {
            Console.WriteLine("Reading resource file");

            string pathForFileResource = Path.Combine(DIR_HELPER_RESOURCES, FILE_RESOURCE);
            if (!File.Exists(pathForFileResource))
            {
                Console.WriteLine("Unable to open resource file");
                return;
            }

            string[] resourceFileContents = File.ReadAllLines(pathForFileResource);
            foreach (string replaceResource in resourceFileContents)
            {
                string fileWithStruct = FindFileWithStruct(replaceResource);
                if (fileWithStruct == null)
                {
                    continue;
                }

                Console.WriteLine("Generating boiler plate for " + replaceResource);

                string replaceResourceManager = replaceResource + "Manager";

                string pathToRemoveFromFileWithStruct = Path.Combine(DIR_SRC, DIR_ENGINE);
                string replaceIncludePath = fileWithStruct.Replace(pathToRemoveFromFileWithStruct, "");
                replaceIncludePath = "../" + replaceIncludePath;
                replaceIncludePath = replaceIncludePath.Replace("\\", "/");
                replaceIncludePath = replaceIncludePath.Replace("//", "/");
                replaceIncludePath = "#include " + '"' + replaceIncludePath + '"';

                string resourceManagerPathForHeader = Path.Combine(DIR_SRC, DIR_ENGINE, DIR_RESOURCES, replaceResourceManager + ".h");
                WriteTemplate(Path.Combine(DIR_HELPER_RESOURCES, FILE_TEMPLATE_RESOURCE_HEADER),
                    resourceManagerPathForHeader, replaceIncludePath, replaceResource, replaceResourceManager);

                string resourceManagerPathForDefinition = Path.Combine(DIR_SRC, DIR_ENGINE, DIR_RESOURCES, replaceResourceManager + ".c");
                WriteTemplate(Path.Combine(DIR_HELPER_RESOURCES, FILE_TEMPLATE_RESOURCE_DEFINITIONS), 
                    resourceManagerPathForDefinition, replaceIncludePath, replaceResource, replaceResourceManager);
            }

            Console.WriteLine("Completed creating resource boiler plate!");

            PrintHelpLines();
        }
        static string FindFileWithStructHelper(string path, string findThisStruct)
        {
            if (!Path.Exists(path))
            {
                Console.WriteLine("Unable to find " + path);
            }

            string[] files = Directory.GetFiles(path);
            foreach (string file in files)
            {
                string extension = Path.GetExtension(file);
                if (extension == ".c" || extension == ".h")
                {
                    string[] fileContents = File.ReadAllLines(file);
                    foreach (string fileContent in fileContents)
                    {
                        string trimmedFileContent = fileContent.Trim();
                        if (trimmedFileContent == findThisStruct)
                        {
                            return file;
                        }
                    }
                }
            }

            string[] directories = Directory.GetDirectories(path);
            foreach (string directory in directories)
            {
                string fileToReturn = FindFileWithStructHelper(directory, findThisStruct);
                if(fileToReturn != null)
                {
                    return fileToReturn;
                }
            }

            return null;
        }
        static string FindFileWithStruct(string findThisStruct)
        {
            string strToFind = "typedef struct " + findThisStruct;
            string path = Path.Combine(DIR_SRC);
            return FindFileWithStructHelper(path, strToFind);
        }
    }
}
