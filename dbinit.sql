create DATABASE `cn_db`;

use `cn_db`;

create table `entries` (
	`id` BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT,
	`topic` VARCHAR(50) NOT NULL, 
	`qtype` VARCHAR(50) NOT NULL, 
	`qtext` TEXT NOT NULL, 
	`ans` VARCHAR(255) NOT NULL, 
	`expln` TEXT NOT NULL
);