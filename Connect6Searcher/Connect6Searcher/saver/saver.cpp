#include "saver.h"

#define Message(m) fprintf(stderr, m)

void SaverGame::SaveToDisk(const char *path)
{
     FILE *file = fopen(path, "r+b");
     if(file == NULL) {
         file = fopen(path, "w");
         fclose(file);
         file = fopen(path, "r+b");
     }

     assert(file != NULL);
     fseek(file, 0, SEEK_END);

     if(ftell(file) == 0) {
         Message("new game data\n");
         DWORD zero = 0;
         DWORD fsz = HEADER_BYTES;
         SaverSummary s;
         fwrite(&zero, sizeof(zero), 1, file);
         fwrite(&fsz, sizeof(fsz), 1, file);

         for(int i=0; i<SUMMARIES_MAX; ++i) {
             fwrite(&s.Data(), sizeof(s.Data()), 1, file);
         }
     }

     DWORD fsz, sums; // File SiZe and number of SUMmarieS
     fseek(file, 0, SEEK_SET);
     fread(&sums, sizeof(sums), 1, file);
     assert(0 <= sums && sums <= SUMMARIES_MAX);
     assert(! feof(file));
     fread(&fsz, sizeof(fsz), 1, file);
     assert(HEADER_BYTES <= fsz && fsz <= POS_MAX);

#ifdef _MSG_SAVE_LOAD
     char msg[1024];
     sprintf(msg, "sums = %d, fsz = %d\n", sums, fsz);
     Message(msg);
#endif // _MSG_SAVE_LOAD

     SaverSummary summary(fsz, (int)_move.size(), _isDraw, _isBlackWinner);
     fseek(file, SUMMARY_POS(sums), SEEK_SET);

#ifdef _MSG_SAVE_LOAD
     sprintf(msg, "summary: pos = %d, moves = %d, %sdraw, winner = %s\n",
         summary.GetPos(),
         summary.GetMoves(),
         summary.IsDraw() ? "" : "not ",
         !summary.IsDraw() && summary.IsBlackWinner() ? "black" : (summary.IsDraw() ? "?" : "white"));
     Message(msg);
     sprintf(msg, "writing summary at pos %d...\n", ftell(file));
     Message(msg);
#endif // _MSG_SAVE_LOAD

     fwrite(&summary.Data(), sizeof(summary.Data()), 1, file);

#ifdef _MSG_SAVE_LOAD
     Message("completed\n");
#endif // _MSG_SAVE_LOAD

     fseek(file, fsz, SEEK_SET);
     for(size_t i=0; i<_move.size(); ++i) {
         fwrite(&_move[i].Data(), sizeof(_move[i].Data()), 1, file);
     }

     fsz = ftell(file);
     ++sums;

#ifdef _MSG_SAVE_LOAD
     sprintf(msg, "%d summaries\n", sums);
     Message(msg);
#endif // _MSG_SAVE_LOAD

     fseek(file, 0, SEEK_SET);

#ifdef _MSG_SAVE_LOAD
     sprintf(msg, "writing sums = %d at pos %d...\n", sums, ftell(file));
     Message(msg);
#endif // _MSG_SAVE_LOAD

     fwrite(&sums, sizeof(sums), 1, file);

#ifdef _MSG_SAVE_LOAD
     sprintf(msg, "write sums completed\nwriting fsz = %d at pos %d...\n", fsz, ftell(file));
     Message(msg);
#endif // _MSG_SAVE_LOAD

     fwrite(&fsz, sizeof(fsz), 1, file);

#ifdef _MSG_SAVE_LOAD
     Message("write fsz completed\n");
#endif // _MSG_SAVE_LOAD

     fclose(file);
}

void SaverGame::LoadFromDisk(const char *path, int n)
{
    FILE *file = fopen(path, "rb");
    assert(file != NULL);
    assert(ftell(file) == 0);
    DWORD fsz, sums; // File SiZe and number of SUMmarieS
    fread(&sums, sizeof(sums), 1, file);
    assert(! feof(file));
    fread(&fsz, sizeof(fsz), 1, file);
    assert(0 <= n && n < (int)sums && (int)sums <= SUMMARIES_MAX);
    assert(HEADER_BYTES <= fsz && fsz <= POS_MAX);

#ifdef _MSG_SAVE_LOAD
    char msg[1024];
    sprintf(msg, "sums = %d, fsz = %d\n", sums, fsz);
    Message(msg);
    sprintf(msg, "seeking %d...\n", SUMMARY_POS(n));
    Message(msg);
#endif // _MSG_SAVE_LOAD

    fseek(file, SUMMARY_POS(n), SEEK_SET);

#ifdef _MSG_SAVE_LOAD
    Message("completed\n");
#endif // _MSG_SAVE_LOAD

    assert(! feof(file));
    SaverSummary s;
    fread(&s.Data(), sizeof(s.Data()), 1, file);
    _isBlackWinner = s.IsBlackWinner();
    _isDraw = s.IsDraw();

#ifdef _MSG_SAVE_LOAD
    sprintf(msg, "seeking %d...\n", s.GetPos());
    Message(msg);
#endif // _MSG_SAVE_LOAD

    fseek(file, s.GetPos(), SEEK_SET);

#ifdef _MSG_SAVE_LOAD
    Message("completed\n");
#endif // _MSG_SAVE_LOAD

    assert(! feof(file));
    _move.clear();

    for(int i=0; i<s.GetMoves(); ++i) {
        assert(! feof(file));
        SaverMove m;
        fread((void *)&m.Data(), sizeof(m.Data()), 1, file);
        _move.push_back(m);
    }

    fclose(file);
}

int SaverGame::GetGamesCount(const char *path)
{
    FILE *file = fopen(path, "rb");
    assert(file != NULL);
    assert(ftell(file) == 0);
    assert(! feof(file));
    DWORD games;
    fread(&games, sizeof(games), 1, file);
    fclose(file);
    return games;
}
