#include "account.hpp"

AcSys::AcSys() : data("accounts.dat", "accounts.rec", "accounts.root") {}

AcSys::~AcSys() = default;

bool AcSys::adduser(const std::string &usr, const std::string &psw,
                    const std::string &name, const std::string &mail,
                    const std::string &cur_usr, int priv) {
  if (data.empty()) { // Add 1st account;
    Account admin(usr, psw, name, mail, 10);
    data.insert(admin);
    return true;
  } else { // normal
    if (usrpriv(cur_usr) <= priv) {
      return false;
    }

    Account new_acc(usr, psw, name, mail, priv);

    return data.insert(new_acc);
  }
}

bool AcSys::login(const std::string &usr, const std::string &psw) {
  if (usrpriv(usr) >= 0) {
    return false;
  }
  Account acc(usr);
  if (data.at(acc)) {
    const int res = strcmp(psw.c_str(), acc.password);
    if (res == 0) {
      log_table.insert({usr, acc.privilege});
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool AcSys::logout(const std::string &usr) {
  auto it = log_table.find(usr);
  if (it == log_table.end()) {
    return false;
  } else {
    log_table.erase(it);
    return true;
  }
}

std::string AcSys::query(const std::string &cur_usr, const std::string &usr) {
  const int cur_priv = usrpriv(cur_usr);
  if (cur_priv == -1) {
    return "";
  }

  Account acc(usr);
  if (data.at(acc)) {
    if (acc.privilege <= cur_priv) {
      std::string res = std::string(acc.username) + " " +
                        std::string(acc.name) + " " + std::string(acc.mail) +
                        " " + std::to_string(acc.privilege);
      return res;

    } else {
      return "";
    }
  } else {
    return "";
  }
}

std::string AcSys::modify(const std::string &usr, const std::string &psw,
                          const std::string &name, const std::string &mail,
                          const std::string &cur_usr, int priv) {
  const int cur_priv = usrpriv(cur_usr);
  if (cur_priv == -1) {
    return "";
  }

  Account acc(usr);
  if (data.at(acc)) {
    if (acc.privilege <= cur_priv && priv < cur_priv) {
      if (psw != "") {
        strcpy(acc.password, psw.c_str());
      }
      if (name != "") {
        strcpy(acc.name, name.c_str());
      }
      if (mail != "") {
        strcpy(acc.mail, mail.c_str());
      }
      if (priv != -1) {
        acc.privilege = priv;
      }

      data.modify(acc);
      std::string res = std::string(acc.username) + " " +
                        std::string(acc.name) + " " + std::string(acc.mail) +
                        " " + std::to_string(acc.privilege);
      return res;

    } else {
      return "";
    }
  } else {
    return "";
  }
}

int AcSys::usrpriv(
    const std::string &usr) const { // 0-10 if logged in, -1 if not
  auto it = log_table.find(usr);
  if (it == log_table.cend()) {
    return -1;
  } else {
    return it->second;
  }
}

void AcSys::clear() {
  data.clear();
  log_table.clear();
}