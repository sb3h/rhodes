class Fixnum

  alias quof fdiv
  alias rdiv quo

  alias power! ** unless method_defined? :power!
  alias rpower **

end

class Bignum

  alias quof fdiv
  alias rdiv quo

  alias power! ** unless method_defined? :power!
  alias rpower **

end
