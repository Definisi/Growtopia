/*
  Warnings:

  - Added the required column `name` to the `bots` table without a default value. This is not possible if the table is not empty.

*/
-- RedefineTables
PRAGMA defer_foreign_keys=ON;
PRAGMA foreign_keys=OFF;
CREATE TABLE "new_bots" (
    "id" TEXT NOT NULL PRIMARY KEY,
    "clientId" TEXT NOT NULL,
    "name" TEXT NOT NULL,
    "username" TEXT NOT NULL,
    "world" TEXT,
    "status" TEXT NOT NULL DEFAULT 'OFFLINE',
    "position" JSONB,
    "inventory" JSONB,
    "lastSeen" DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "createdAt" DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" DATETIME NOT NULL,
    "nodeId" TEXT NOT NULL,
    CONSTRAINT "bots_nodeId_fkey" FOREIGN KEY ("nodeId") REFERENCES "nodes" ("id") ON DELETE CASCADE ON UPDATE CASCADE
);
INSERT INTO "new_bots" ("clientId", "createdAt", "id", "inventory", "lastSeen", "nodeId", "position", "status", "updatedAt", "username", "world", "name") SELECT "clientId", "createdAt", "id", "inventory", "lastSeen", "nodeId", "position", "status", "updatedAt", "username", "world", "username" FROM "bots";
DROP TABLE "bots";
ALTER TABLE "new_bots" RENAME TO "bots";
CREATE UNIQUE INDEX "bots_clientId_key" ON "bots"("clientId");
PRAGMA foreign_keys=ON;
PRAGMA defer_foreign_keys=OFF;
