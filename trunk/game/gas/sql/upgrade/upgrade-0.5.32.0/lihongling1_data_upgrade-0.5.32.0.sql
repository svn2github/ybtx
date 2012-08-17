update tbl_mail set m_uMoney = round(m_uMoney*0.95) where cs_uSenderID is not null;

