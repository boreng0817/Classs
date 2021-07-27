SELECT name FROM Pokemon
WHERE type='Grass' ORDER BY name;

SELECT name FROM Trainer
WHERE hometown='Brown City' OR hometown='Rainbow City' ORDER BY name;

SELECT  DISTINCT type FROM Pokemon 
ORDER BY type;

SELECT name FROM City
WHERE name LIKE 'B%' ORDER BY name;

SELECT hometown FROM Trainer
WHERE name NOT LIKE 'M%' ORDER BY hometown;

SELECT nickname 
FROM CatchedPokemon C
WHERE C.level=(SELECT MAX(level) FROM CatchedPokemon);

SELECT name FROM Pokemon
WHERE name LIKE 'A%' OR name LIKE 'E%' OR name LIKE 'I%' OR name LIKE 'O%' OR name LIKE 'U%'
ORDER BY name;

SELECT AVG(level) FROM CatchedPokemon;

SELECT MAX(C.level) FROM Trainer AS T, CatchedPokemon AS C
WHERE T.id = C.owner_id AND T.name='Yellow';

SELECT DISTINCT hometown FROM Trainer
ORDER BY hometown;

SELECT T.name, C.nickname FROM Trainer T, Pokemon P, CatchedPokemon C
WHERE P.id=C.pid AND T.id=C.owner_id AND C.nickname LIKE 'A%' ORDER BY T.name;

SELECT T.name FROM Trainer T, Gym G, City C
WHERE G.city=C.name AND G.leader_id=T.id AND C.description='Amazon';

SELECT T.id AS '#13', COUNT(C.id) FROM Trainer T, CatchedPokemon C, Pokemon P
WHERE T.id=C.owner_id AND P.id=C.pid AND P.type='Fire'
GROUP BY T.id
ORDER BY COUNT(C.id) DESC LIMIT 1;

SELECT DISTINCT type FROM Pokemon
WHERE id<10 ORDER BY id DESC;

SELECT COUNT(*) FROM Pokemon
WHERE type NOT LIKE 'Fire';

SELECT P.name AS '#16' FROM Pokemon P, Evolution E
WHERE P.id=E.before_id AND E.before_id > E.after_id ORDER BY P.name;

SELECT AVG(level) FROM CatchedPokemon C, Pokemon P
WHERE C.pid=P.id AND P.type='Water';

SELECT C.nickname 
FROM  Gym G, Trainer T, CatchedPokemon C 
WHERE G.leader_id=T.id AND C.owner_id=T.id ORDER BY C.level DESC LIMIT 1;

SELECT T.name AS '#19' FROM Trainer T, CatchedPokemon C 
WHERE T.id=C.owner_id AND T.hometown = 'Blue CIty'
GROUP BY T.name
ORDER BY AVG(C.level) DESC LIMIT 1;

SELECT P.name 
FROM (  SELECT T.id AS Tid, COUNT(T.id) AS cnt FROM Trainer T
	GROUP BY T.hometown ) m, Pokemon P, CatchedPokemon C
WHERE m.cnt=1 AND C.owner_id=m.Tid AND C.pid=P.id AND P.type='Electric' AND P.id IN (SELECT before_id FROM Evolution);

SELECT T.name, SUM(C.level) FROM Gym G, Trainer T, CatchedPokemon C
WHERE G.leader_id=T.id AND C.owner_id=T.id
GROUP BY T.name
ORDER BY SUM(C.level) DESC;

SELECT C.name FROM Trainer T, City C
WHERE T.hometown=C.name  
GROUP BY C.name
ORDER BY COUNT(T.name) DESC LIMIT 1;

SELECT DISTINCT P.name FROM CatchedPokemon C, Pokemon P, Trainer T
WHERE T.id=C.owner_id AND C.pid=P.id AND T.hometown='Sangnok City' 
AND p.name IN ( SELECT DISTINCT P.name FROM CatchedPokemon C, Pokemon P, Trainer T
	WHERE T.id=C.owner_id AND C.pid=P.id AND T.hometown='Brown City')
ORDER BY P.name;

SELECT T.name FROM Trainer T, Pokemon P, CatchedPokemon C 
WHERE T.id=C.owner_id AND P.id=C.pid AND T.hometown='Sangnok City' AND P.name LIKE 'P%'
ORDER BY T.name;

SELECT T.name, P.name FROM Trainer T, Pokemon P, CatchedPokemon C
WHERE P.id=C.pid AND C.owner_id=T.id
ORDER BY T.name, P.name;

SELECT P.name FROM Pokemon P, Evolution E 
WHERE P.id=E.before_id AND E.after_id NOT IN (SELECT before_id FROM Evolution) AND E.before_id NOT IN (SELECT after_id FROM Evolution)
ORDER BY P.name;

SELECT C.nickname FROM Gym G, Trainer T, CatchedPokemon C, Pokemon P
WHERE G.leader_id=T.id AND T.id=C.owner_id AND C.pid=P.id AND G.city='Sangnok City' AND P.type='Water'
ORDER BY C.nickname;

SELECT m.name AS '#28' 
FROM (  SELECT T.name AS name, COUNT(C.id) AS cnt FROM Trainer T, CatchedPokemon C, Evolution E
	WHERE T.id=C.owner_id AND C.pid=E.after_id
	GROUP BY T.name ) m
WHERE m.cnt > 2
ORDER BY m.name;

SELECT name AS '#29' FROM Pokemon
WHERE id NOT IN ( SELECT DISTINCT pid FROM CatchedPokemon)
ORDER BY name;

SELECT MAX(level) FROM Trainer T, CatchedPokemon C
WHERE T.id=C.owner_id
GROUP BY T.hometown
ORDER BY MAX(level) DESC;

SELECT m.id1 AS id1, m.name AS name1, P.name AS name2, P1.name AS name3
FROM (  SELECT P.name AS name, E.before_id AS id1, E.after_id AS id2 FROM Pokemon P, Evolution E
	WHERE P.id=E.before_id ) m, Pokemon P, Evolution E, Pokemon P1
WHERE m.id2=E.before_id AND P.id=m.id2 AND P1.id=E.after_id
ORDER BY E.after_id;
