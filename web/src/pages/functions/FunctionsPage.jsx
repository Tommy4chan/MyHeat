import { Functions } from '@/components/Functions';
import { FunctionSettings } from './components/FunctionSettings';

const FunctionsPage = () => {
  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 items-start gap-6">
      <Functions isTwoCols={false} />
      <FunctionSettings />
    </div>
  );
};

export default FunctionsPage;