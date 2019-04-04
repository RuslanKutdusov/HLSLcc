#include <string>
#include "hlslcc.h"


int fileExists(const char* path)
{
    FILE* shaderFile;
    shaderFile = fopen(path, "rb");

    if (shaderFile)
    {
        fclose(shaderFile);
        return 1;
    }
    return 0;
}


GLLang LanguageFromString(const char* str)
{
    if (strcmp(str, "330") == 0)
    {
        return LANG_330;
    }
    if (strcmp(str, "400") == 0)
    {
        return LANG_400;
    }
    if (strcmp(str, "410") == 0)
    {
        return LANG_410;
    }
    if (strcmp(str, "420") == 0)
    {
        return LANG_420;
    }
    if (strcmp(str, "430") == 0)
    {
        return LANG_430;
    }
    if (strcmp(str, "440") == 0)
    {
        return LANG_440;
    }
    if (strcmp(str, "metal") == 0)
    {
        return LANG_METAL;
    }
    return LANG_DEFAULT;
}


struct Options
{
    GLLang language;
    int flags;
    const char* shaderFile;
    char* outputShaderFile;
};


void InitOptions(Options* psOptions)
{
    psOptions->language = LANG_DEFAULT;
    psOptions->flags = HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT;
    psOptions->shaderFile = nullptr;
    psOptions->outputShaderFile = nullptr;
}


void PrintHelp()
{
    printf("Command line options:\n");

    printf("\t-lang=X \t GLSL language to use - 330, 400, 410, 420, 430, 440, metal.\n");
    printf("\t-flags=X \t The integer value of the HLSLCC_FLAGS to used.\n");
    printf("\t-in=X \t Shader file to compile.\n");
    printf("\t-out=X \t File to write the compiled shader from -in to.\n");

    printf("\n");
}


int GetOptions(int argc, char** argv, Options* psOptions)
{
    int i;
    int fullShaderChain = -1;
    int hashOut = 0;

    InitOptions(psOptions);

    for (i = 1; i < argc; i++)
    {
        char *option;

        option = strstr(argv[i], "-help");
        if (option != NULL)
        {
            PrintHelp();
            return 0;
        }

        option = strstr(argv[i], "-lang=");
        if (option != NULL)
        {
            psOptions->language = LanguageFromString((&option[strlen("-lang=")]));
        }

        option = strstr(argv[i], "-flags=");
        if (option != NULL)
        {
            psOptions->flags = atol(&option[strlen("-flags=")]);
        }

        option = strstr(argv[i], "-in=");
        if (option != NULL)
        {
            fullShaderChain = 0;
            psOptions->shaderFile = option + strlen("-in=");
            if (!fileExists(psOptions->shaderFile))
            {
                printf("Invalid path: %s\n", psOptions->shaderFile);
                return 0;
            }
        }

        option = strstr(argv[i], "-out=");
        if (option != NULL)
        {
            fullShaderChain = 0;
            psOptions->outputShaderFile = option + strlen("-out=");
        }
    }

    return 1;
}


int main(int argc, char** argv)
{
    Options options;

    if (!GetOptions(argc, argv, &options))
    {
        return 1;
    }

    if (options.shaderFile)
    {
        FILE* outputFile;
        GLSLShader result;
        int compiledOK = 0;

        GlExtensions ext;
        ext.ARB_explicit_attrib_location = 1;
        ext.ARB_explicit_uniform_location = 1;
        ext.ARB_shading_language_420pack = 0;
        ext.OVR_multiview = 0;
        ext.EXT_shader_framebuffer_fetch = 0;

        HLSLccSamplerPrecisionInfo samplerPrecInfo;
        HLSLccReflection reflectionCallbacks;
        compiledOK = TranslateHLSLFromFile(options.shaderFile, options.flags, options.language, &ext, nullptr, samplerPrecInfo, reflectionCallbacks, &result);

        if (compiledOK)
        {
            if (options.outputShaderFile)
            {
                //Dump to file
                outputFile = fopen(options.outputShaderFile, "w");
                fputs(result.sourceCode.c_str(), outputFile);
                fclose(outputFile);
            }
            printf("OK\n");

            return 0;
        }
        else
        {
            printf("Failed\n");

            return 1;
        }
    }

    return 0;
}
