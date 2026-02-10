@echo off
::##############################################################################
:: target-runner para irace (Windows)
::##############################################################################

:: Caminho para o executavel (verifique se esta correto)
SET "exe=.\x64\Release\CaRS_MetaH.exe"

:: Parametros fixos que nao variam (opcional)
SET "fixed_params="

::------------------------------------------------------------------------------
:: 1. Leitura dos Parametros do irace
:: %1 = Candidate ID
:: %2 = Instance ID
:: %3 = Seed
:: %4 = Instance File Path
:: %5... = Parametros de ajuste (ex: --temp 100 --alpha 0.9)
::------------------------------------------------------------------------------

SET candidate=%1
SET instance_id=%2
SET seed=%3
SET instance=%4

:: Remove os 4 primeiros argumentos para sobrar apenas os parametros do algoritmo
SHIFT
SHIFT
SHIFT
SHIFT

:: Agora %1, %2... sao os parametros do algoritmo (--alpha, etc)
:: O loop abaixo captura todos eles numa unica variavel
SET candidate_parameters=
:loop_args
if "%1"=="" goto end_args
SET candidate_parameters=%candidate_parameters% %1
SHIFT
goto loop_args
:end_args

:: Arquivos temporarios para capturar a saida
SET "stdout=c%candidate%-%instance_id%-%seed%.stdout"
SET "stderr=c%candidate%-%instance_id%-%seed%.stderr"

::------------------------------------------------------------------------------
:: 2. Execucao do Algoritmo
:: Redirecionamos stdout (1>) e stderr (2>) para arquivos para nao sujar a saida pro irace.
:: O irace espera receber APENAS UM NUMERO na saida padrao.
::------------------------------------------------------------------------------

"%exe%" %fixed_params% %instance% --seed %seed% %candidate_parameters% > "%stdout%" 2> "%stderr%"

:: Verifica erro de execucao (opcional)
if %errorlevel% neq 0 (
    :: Se der erro, retorna um custo gigante para o irace descartar essa configuracao
    echo 999999999
    goto cleanup
)

::------------------------------------------------------------------------------
:: 3. Parsing da Saida (Leitura do Custo)
:: Assume-se que o seu programa C++ imprime o valor da FO na ULTIMA LINHA.
:: Este loop le o arquivo inteiro, linha por linha. A variavel COST ficara com o valor da ultima.
::------------------------------------------------------------------------------

SET "COST="
FOR /F "usebackq tokens=*" %%a IN ("%stdout%") DO (
    SET COST=%%a
)

:: Verificacao de seguranca: Se COST estiver vazio, algo deu errado
if "%COST%"=="" (
    :: Retorna custo alto para indicar falha sem quebrar o irace
    echo 999999999
) else (
    :: Imprime APENAS o numero para o irace
    echo %COST%
)

::------------------------------------------------------------------------------
:: 4. Limpeza
::------------------------------------------------------------------------------
:cleanup
del "%stdout%" "%stderr%" 2>nul
exit 0