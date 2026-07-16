-- CREATE TABLE sessions (
--	start DATETIME DEFAULT CURRENT_TIMESTAMP,
--	end DATETIME,
--	startenergy INTEGER,
--	endenergy INTEGER,
--	difficulty INTEGER,
--	focusdepth INTEGER
-- );

-- DROP TABLE sessions;

-- CREATE TABLE sessions (
--      start DATETIME DEFAULT CURRENT_TIMESTAMP,
--      end DATETIME,
--      startenergy INTEGER,
--      endenergy INTEGER,
--      difficulty INTEGER,
--      focusdepth INTEGER
-- );

-- INSERT INTO sessions (startenergy)
-- VALUES (4);

-- ALTER TABLE sessions ADD COLUMN id INTEGER;
-- UPDATE sessions SET endenergy = 4

-- DROP TABLE sessions;

-- CREATE TABLE sessions (
--	id INTEGER PRIMARY KEY,
--	start DATETIME DEFAULT CURRENT_TIMESTAMP,
--	end DATETIME,
--    	startenergy INTEGER,
--   	endenergy INTEGER,
--   	difficulty INTEGER,
--   	focusdepth INTEGER
-- );

--INSERT INTO sessions (startenergy)
--VALUES (4);

-- UPDATE sessions SET endenergy = 4, difficulty = 1, focusdepth = 3 WHERE id = 1;

-- UPDATE sessions SET end = CURRENT_TIMESTAMP WHERE id = 1;
-- SELECT * FROM sessions;

-- DROP TABLE sessions;

--CREATE TABLE IF NOT EXISTS sessions (
--                id INTEGER PRIMARY KEY AUTOINCREMENT,
 --               start DATETIME DEFAULT CURRENT_TIMESTAMP,
 --               end DATETIME,
--                startenergy INTEGER,
--                endenergy INTEGER,
--                difficulty INTEGER,
--                focusdepth INTEGER
--);

-- INSERT INTO sessions (startenergy, endenergy)
-- VALUES (10, 6);

--UPDATE sessions SET end = CURRENT_TIMESTAMP WHERE id = 2;
--UPDATE sessions SET difficulty = 2 WHERE id = 2;
--UPDATE sessions SET focusdepth = 1 WHERE id = 2;

--SELECT 
--    id, start 
--FROM 
--    sessions
--ORDER BY
--    end DESC;

-- refer to https://www.sqlitetutorial.net/sqlite-distinct/ for a good tutorial

-- SELECT * FROM sessions;

-- WHERE can be "column = x" "column IN (1, 2, 3)" "column LIKE 'An%'" or "column BETWEEN 10 AND 20"

--selects only the latest entry
-- SELECT 
--    id, start
-- FROM   
--    sessions
-- ORDER BY 
--    end DESC
-- LIMIT 1;

-- ALTER TABLE sessions ADD COLUMN status INTEGER DEFAULT 0;