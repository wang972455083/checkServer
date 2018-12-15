/*
Navicat MySQL Data Transfer

Source Server         : 本机
Source Server Version : 50520
Source Host           : localhost:3306
Source Database       : center

Target Server Type    : MYSQL
Target Server Version : 50520
File Encoding         : 65001

Date: 2017-04-05 15:50:45
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `admin`
-- ----------------------------
DROP TABLE IF EXISTS `admin`;
CREATE TABLE `admin` (
  `Name` varchar(50) NOT NULL,
  `Pass` varchar(50) DEFAULT NULL,
  `Type` int(11) DEFAULT '0',
  `Card1` int(11) DEFAULT '0',
  `Card2` int(11) DEFAULT '0',
  `Card3` int(11) DEFAULT '0',
  `Time` int(11) DEFAULT '0',
  `Nike` varchar(50) DEFAULT NULL,
  `Invite` int(11) DEFAULT '0',
  `Info` varchar(256) DEFAULT '',
  PRIMARY KEY (`Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for `bill`
-- ----------------------------
DROP TABLE IF EXISTS `bill`;
CREATE TABLE `bill` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `AdminName` varchar(50) DEFAULT NULL,
  `CardType` int(11) DEFAULT NULL,
  `CardNum` int(11) DEFAULT NULL,
  `Time` int(11) DEFAULT NULL,
  `Flag` int(11) DEFAULT '1' COMMENT ' 1-冲值,2-赠送 ',
  `Type` int(11) DEFAULT '1' COMMENT ' 1-会员,2-玩家 ',
  `DesName` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for `charge`
-- ----------------------------
DROP TABLE IF EXISTS `charge`;
CREATE TABLE `charge` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `Time` int(11) DEFAULT NULL,
  `CardType` int(11) DEFAULT NULL,
  `CardNum` int(11) DEFAULT NULL,
  `Type` int(11) DEFAULT NULL,
  `Admin` varchar(50) DEFAULT '',
  `UserId` int(11) DEFAULT NULL,
  PRIMARY KEY (`Id`),
  KEY `UserId` (`UserId`),
  KEY `Time` (`Time`),
  KEY `Admin` (`Admin`),
  KEY `Type` (`Type`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=utf8;

-- -------------------------------
--
-- -------------------------------
CREATE TABLE `charge_201709` (
  `Id` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增主键',
  `ServerId` int(11) NOT NULL COMMENT '玩家所属区服ID',
  `UserId` int(11) NOT NULL COMMENT '玩家ID',
  `Time` int(11) NOT NULL COMMENT '时间',
  `CardNum` int(11) NOT NULL COMMENT '房卡变动数量',
  `Type` smallint(6) NOT NULL COMMENT '房卡变动类型',
  `Balance` int(11) NOT NULL COMMENT '玩家房卡余额',
  `Operator` varchar(50) NOT NULL DEFAULT '' COMMENT '操作者',
  `Remark` varchar(50) NOT NULL DEFAULT '' COMMENT '备注',
  PRIMARY KEY (`Id`),
  KEY `idx_uid_time` (`UserId`,`Time`)
) ENGINE=InnoDB AUTO_INCREMENT=212 DEFAULT CHARSET=utf8mb4;
-- ----------------------------
-- Table structure for `onlinelog`
-- ----------------------------
DROP TABLE IF EXISTS `onlinelog`;
CREATE TABLE `onlinelog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `DateTime` int(11) NOT NULL,
  `serverID` int(11) NOT NULL DEFAULT '0',
  `serverName` varchar(128) NOT NULL DEFAULT '',
  `onlineCount` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2798 DEFAULT CHARSET=utf8;


-- ----------------------------
-- Table structure for `user`
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `OpenId` varchar(64) COLLATE utf8mb4_bin NOT NULL,
  `Nike` varchar(64) COLLATE utf8mb4_bin NOT NULL,
  `Sex` int(11) DEFAULT '0',
  `Provice` varchar(64) COLLATE utf8mb4_bin DEFAULT '',
  `City` varchar(64) COLLATE utf8mb4_bin DEFAULT '',
  `Country` varchar(64) COLLATE utf8mb4_bin DEFAULT '',
  `HeadImageUrl` varchar(512) COLLATE utf8mb4_bin DEFAULT '',
  `UnionId` varchar(64) COLLATE utf8mb4_bin NOT NULL DEFAULT '',
  `NumsCard1` int(11) DEFAULT '0',
  `NumsCard2` int(11) unsigned DEFAULT '0',
  `NumsCard3` int(11) DEFAULT '0',
  `LastLoginTime` int(11) DEFAULT '0',
  `RegTime` int(11) DEFAULT '0',
  `New` int(11) DEFAULT '0',
  `Gm` int(11) DEFAULT '0',
  `TotalCardNum` int(11) DEFAULT '0',
  `TotalPlayNum` int(11) DEFAULT '0',
  `location` varchar(512) COLLATE utf8mb4_bin DEFAULT '',
  `last_relief_time` int(11) DEFAULT '0',
  `phoneNumber` varchar(512) COLLATE utf8mb4_bin DEFAULT '',
  `serverID` int(11) DEFAULT '33001',
  PRIMARY KEY (`Id`),
  KEY `NumsCard1` (`NumsCard1`),
  KEY `NumsCard2` (`NumsCard2`),
  KEY `NumsCard3` (`NumsCard3`),
  KEY `LastLoginTime` (`LastLoginTime`),
  KEY `RegTime` (`RegTime`),
  KEY `OpenId` (`OpenId`),
  KEY `UnionId` (`UnionId`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;

-- ----------------------------
-- Table structure for `useractive`
-- ----------------------------
DROP TABLE IF EXISTS `useractive`;
CREATE TABLE `useractive` (
  `DateTime` int(11) NOT NULL,
  `NewUser` int(11) DEFAULT '0',
  `ActiveUser` int(11) DEFAULT '0',
  `PlayCount` int(11) DEFAULT '0',
  PRIMARY KEY (`DateTime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of useractive
-- ----------------------------
-- 分享送房卡活动
DROP TABLE IF EXISTS `Activity`;
CREATE TABLE `Activity` (
  `Id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '自增长主键ID',
  `userid` int(11) NOT NULL  COMMENT '玩家ID',
  `fetchCnt` int(11) NOT NULL DEFAULT '0' COMMENT '领取次数',
  `lastFetchTime` int(11) NOT NULL DEFAULT '0'   COMMENT  '上一次领取时间',
  PRIMARY KEY (`Id`),
  UNIQUE KEY (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin ;
---------------------
--随机id
---------------------
DROP TABLE IF EXISTS 'user_id_table';
CREATE TABLE `user_id_table` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `UserId` int(11) NOT NULL,
  PRIMARY KEY (`Id`),
  UNIQUE KEY `UserId` (`UserId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;

-------------------------
---公会表
------------------------
CREATE TABLE `team` (
  `Id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '自增主键',
  `TeamID` int(11) NOT NULL COMMENT '工会ID',
  `CreaterID` int(11) NOT NULL COMMENT '创建者ID',
  `adminId` int(11) NOT NULL COMMENT '会主ID',
  `serverID` int(11) NOT NULL COMMENT '服务器ID',
  `CreaterName` varchar(200) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '创建者名称',
  `TeamName` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '工会名称',
  `Cards` int(11) NOT NULL COMMENT '工会房卡',
  `CardAlert` int(11) NOT NULL DEFAULT '300' COMMENT '工会房卡低于改值报警',
  `PerDayMemLimit` int(11) NOT NULL DEFAULT '100' COMMENT '成员每天最大可使用工会房卡数量',
  `CreateTime` int(11) NOT NULL DEFAULT '0' COMMENT '工会创建时间戳',
  `TeamState` int(11) NOT NULL COMMENT '工会状态',
  `boardInfo` varchar(200) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT '' COMMENT '公告信息',
  `DisbandedTime` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Id`),
  KEY `TeamID` (`TeamID`),
  KEY `adminId` (`adminId`)
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8 COMMENT='存储工会会长创建的工会';

-----------------
---公会充值表(按月分表)
-----------------
CREATE TABLE `team_charge_xxxxx` (
  `Id` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增主键',
  `ServerId` int(11) NOT NULL COMMENT '工会所属区服ID',
  `TeamId` int(11) NOT NULL COMMENT '工会ID',
  `TeamMemId` int(11) NOT NULL COMMENT '成员ID',
  `Time` int(11) NOT NULL COMMENT '时间',
  `CardNum` int(11) NOT NULL COMMENT '房卡变动数量',
  `Type` smallint(6) NOT NULL COMMENT '房卡变动类型',
  `Balance` int(11) NOT NULL COMMENT '工会房卡余额',
  `Operator` varchar(50) NOT NULL DEFAULT '' COMMENT '操作者',
  `Remark` varchar(50) NOT NULL DEFAULT '' COMMENT '备注',
  PRIMARY KEY (`Id`),
  KEY `idx_tid_mid_time` (`TeamId`,`TeamMemId`,`Time`),
  KEY `idx_mid_time` (`TeamMemId`,`Time`)
) ENGINE=InnoDB AUTO_INCREMENT=24 DEFAULT CHARSET=utf8mb4;

-------------------
--team随机ID表
------------------
CREATE TABLE `team_id_table` (
  `Id` int(11) NOT NULL AUTO_INCREMENT COMMENT '主键',
  `TeamId` int(11) NOT NULL DEFAULT '0' COMMENT '工会ID',
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=931979 DEFAULT CHARSET=utf8;

-----------------
---team_log表
-----------------
CREATE TABLE `team_log` (
  `Id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '自增主键ID',
  `DeskID` int(11) NOT NULL DEFAULT '0' COMMENT '桌子ID',
  `TeamID` int(11) NOT NULL DEFAULT '0' COMMENT '工会ID',
  `UserId1` int(11) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `UserId2` int(11) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `UserId3` int(11) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `UserId4` int(11) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `Score1` int(11) NOT NULL DEFAULT '0' COMMENT '得分',
  `Score2` int(11) NOT NULL DEFAULT '0' COMMENT '得分',
  `Score3` int(11) NOT NULL DEFAULT '0' COMMENT '得分',
  `Score4` int(11) NOT NULL DEFAULT '0' COMMENT '得分',
  `Nike1` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `HeadImageUrl1` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT '',
  `Nike2` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `HeadImageUrl2` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT '',
  `Nike3` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `HeadImageUrl3` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT '',
  `Nike4` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `HeadImageUrl4` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT '',
  `OptState` int(11) NOT NULL DEFAULT '0' COMMENT '是否已阅',
  `PlayFlag` int(11) DEFAULT '0' COMMENT '玩法类型',
  `Time` int(11) NOT NULL DEFAULT '0' COMMENT '时间戳',
  `serverID` int(11) NOT NULL COMMENT '服务器ID',
  PRIMARY KEY (`Id`),
  KEY `TeamID` (`TeamID`),
  KEY `Indx_time_teamid` (`Time`,`TeamID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='记录使用工会房间的开局日志';

-----------------------
--公会成员操作log
-----------------------
CREATE TABLE `team_mem_opt` (
  `Id` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增主键',
  `TeamID` int(11) NOT NULL DEFAULT '0' COMMENT '工会ID',
  `TeamName` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '工会名称',
  `UserID` int(11) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `Opt` tinyint(4) NOT NULL DEFAULT '0' COMMENT '操作类型',
  `OptTime` int(11) NOT NULL DEFAULT '0' COMMENT '操作的时间戳',
  PRIMARY KEY (`Id`),
  KEY `indx_TeamId` (`TeamID`),
  KEY `indx_UserID_TeamID` (`UserID`,`TeamID`)
) ENGINE=InnoDB AUTO_INCREMENT=79 DEFAULT CHARSET=utf8 COMMENT='玩家加入离开工会的日志表';

--------------
--公会成员表
--------------
CREATE TABLE `team_member` (
  `Id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '自增主键',
  `UserID` int(11) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `TeamID` int(11) NOT NULL DEFAULT '0' COMMENT '工会ID',
  `serverID` int(11) NOT NULL COMMENT '服务器ID',
  `UseCardNum` int(11) NOT NULL DEFAULT '0' COMMENT '当天房卡使用量',
  `TeamState` int(11) NOT NULL DEFAULT '0' COMMENT '玩家在工会中的状态',
  `LastUserCardTime` int(11) NOT NULL DEFAULT '0' COMMENT '最后一次使用工会房卡的时间戳',
  `JoinTime` int(11) NOT NULL DEFAULT '0' COMMENT '加入工会的时间戳',
  `ReqTime` int(11) NOT NULL DEFAULT '0' COMMENT '申请退出工会的时间戳',
  `WinerCount` int(11) DEFAULT '0' COMMENT '大赢家次数',
  `TotalUseCardNum` int(11) DEFAULT '0' COMMENT '使用俱乐部房卡总数',
  `TotalCreateDeskCount` int(11) DEFAULT '0' COMMENT '通过俱乐部创建房间的次数',
  `TotalPlayCount` int(11) DEFAULT '0' COMMENT '参与俱乐部房间游戏的次数',
  `QuitTime` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Id`),
  KEY `TeamID` (`TeamID`),
  KEY `Indx_UserID_TeamID` (`UserID`,`TeamID`)
) ENGINE=InnoDB AUTO_INCREMENT=47 DEFAULT CHARSET=utf8 COMMENT='存储工会成员';

--------------
--封禁用户表
--------------
CREATE TABLE IF NOT EXISTS `block_user` (
  `Id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '自增主键',
  `UserID` int(11) NOT NULL COMMENT '用户ID',
  `StartTime` int(11) NOT NULL DEFAULT '0' COMMENT '封禁开始时间戳',
  `EndTime` int(11) NOT NULL DEFAULT '0' COMMENT '封禁结束时间戳，-1为永久封禁',
  PRIMARY KEY (`Id`),
  UNIQUE KEY `UserID` (`UserID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='封禁用户表';



