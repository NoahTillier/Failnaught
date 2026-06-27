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
--    	endenergy INTEGER,
--   	difficulty INTEGER,
--   	focusdepth INTEGER
-- );

--INSERT INTO sessions (startenergy)
--VALUES (4);

-- UPDATE sessions SET endenergy = 4, difficulty = 1, focusdepth = 3 WHERE id = 1;

UPDATE sessions SET end = CURRENT_TIMESTAMP WHERE id = 1;
SELECT * FROM sessions;
